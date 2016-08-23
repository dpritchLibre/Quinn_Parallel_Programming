
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mpi_helper.h"

#define NOT_MARK 0  // not yet marked as having a factor
#define YES_MARK 1  // has been marked as having a factor




/* Given set starting value startval, ending value endval, rank of process rank,
 * and number of processes size: calculate the smallest odd number in the
 * rank-th set, the largest number in the rank-th set, and the number of odd
 * values in the set, and store these values in *low_value, *high_value, and
 * *set_size, respectively.
 */

void local_set_params(int startval, int endval, int rank, int size, 
		      int *low_value, int *high_value, int *set_size) {

    int n_elem = endval - startval + 1;

    // Find the lowest odd value in the rank-th set.  Note that there are
    *low_value = BLOCK_LOW(rank, size, n_elem) + startval;
    if (!(*low_value % 2)) {
	(*low_value)++;
    }
    // Find the largest value in the rank-th set (even number is okay)
    *high_value = BLOCK_HIGH(rank, size, n_elem) + startval;

    /* Find the amount of odd numbers between low_value and high_value,
     * inclusive
     */
    *set_size = ((*high_value - *low_value) / 2) + 1;
}




/* Find the first odd valued multiple of m that is >= to both m^2 and to bnd,
 * and return the number of odd numbers past bnd that it is.
 *
 * PRE: assumes m is an odd number, and that m^2 is representable as an integer
 *
 * EXAMPLE:  m = 5, bnd =  15;  return 5 (odd past, i.e. 17, 19, 21, 23, 25)
 * EXAMPLE:  m = 9, bnd =  95;  return 2 (odd past, i.e. 97, 99)
 * EXAMPLE:  m = 3, bnd =   9;  return 3 (odd past, i.e. 11, 13, 15)
 * EXAMPLE:  m = 7, bnd = 343;  return 0 (7 * 7 * 7 = 343)
 */

int num_odd_past(int m, int bnd) {

    int quot;
    int npast;

    // The number of times that m goes into bnd
    quot = bnd / m;

    /* case: m^2 > bnd.  Find how far past bnd m^2 is, and then divide by 2 to
     * get the number of odd numbers past.
     */
    if (quot < m) {
	npast = ((m * m) - bnd) / 2;
    }
    /* case: quot is odd and m^2 <= bnd; the smallest odd multiple of m is
     * either equal to bnd or is given by (quot + 2) * m.  Find how far past bnd
     * this number is, and then divide by 2 to get the number of odd numbers
     * past.
     */
    else if (quot % 2) {
	npast = (quot * m) - bnd;
	// case: bnd is not a multiple of m; add 2 * m
	if (npast) {
	    npast = (npast + (2 * m)) / 2;
	}
    }
    /* case: quot is even and m^2 < low_number; the smallest odd multiple of m
     * is (quot + 1) * m.  Find how far past bnd this number is, and then divide
     * by 2 to get the number of odd numbers past.
     */
    else {
	npast = (((quot + 1) * m) - bnd) / 2;
    }

    return npast;
}




/* Allocate len bytes of memory and initialize to NOT_MARK, and set *grid to
 * point to the memory location
 */

void initialize_grid(char **grid, int len) {

    /* Allocate memory to store this process's share of the prime number grid.
     * The amount of memory allocated is enough to store a value for every odd
     * number b/w low_value and high_value, inclusive.
     */
    *grid = calloc(len, 1);
    if (*grid == NULL) {
	fprintf(stderr, "error allocating memory for prime number grid\n");
	MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
    }
}




/* Finds the prime elements in the set of odd numbers from {1, ..., rootn} and
 * marks off all odd multiples of the element in the local set >= the square of
 * the element.
 *
 * If this is the 0-th rank process, then finding the primes in set of odd
 * numbers from {1, ..., rootn} is done simultaneously as marking off odd
 * multiples of the primes.  If it is not the 0-th process, then the process
 * waits for a broadcast from the 0-th process for each prime number before
 * marking off multiples.
 */

void fill_grid_v1(char *grid_local, int rootn, int local_low, int local_setsize, int rank) {

    int currval;    // value which me mark multiples of in grid
    int curr_idx;   // index corresp. to currval in array of odds {1, 3, ...}
    int start_idx;  // index in grid_local to start marking of multiples
    int k;

    currval = 3;
    curr_idx = 1;

    /* Each iteration marks all of the multiples of currval (such that the
     * multiple is >= currval^2) in the local grid as having factors, and then
     * finds the next value to use for currval (i.e. the next smallest prime
     * number).
     */
    do {

	/* Calculate the local index of the first odd-valued multiple of currval
	 * in the set that is also greater than currval^2
	 */
	start_idx = num_odd_past(currval, local_low);

	/* Step through the local prime grid marking off multiples of currval as
	 * being not primes.  Note that since the local prime grid doesn't - in
	 * concept - include even numbers, so each step effectively marks off
	 * every other multiple of currval.  This however is as desired, since
	 * every second multiple of currval is an even number.
	 */
	for (k = start_idx; k < local_setsize; k += currval) {
	    grid_local[k] = YES_MARK;
	}

	/* case: 0-th process.  This is the process in charge of finding the
	 * next value to start marking of multiples of.
	 */
	if (!rank) {

	    /* Walk through the beginning of the prime number grid to find the
	     * index of the value that we will begin marking off multiples of in
	     * the next iteration.
	     */
	    while (++curr_idx < local_setsize) {
		// case: value corresp. to grid elem. not marked as having a factor
		if (! grid_local[curr_idx]) {
		    break;
		}
	    }

	    // Translate index to the value that the index represents
	    currval = (2 * curr_idx) + 1;
	}

	/* Broadcast the next next value to start marking of multiples of to
	 * remaining processes
	 */
	MPI_Bcast(&currval, 1, MPI_INT, 0, MPI_COMM_WORLD);

    } while (currval <= rootn);

}




/* Find the odd values in the set {1, ..., rootn} that have factors and flag the
 * corresponding elements in the array grid_rootn with the value YES_MARK.
 */

void fill_grid_rootn(char *grid_rootn, int rootn, int rootn_setsize) {

    int currval;    // value which me mark multiples of in grid
    int curr_idx;   // index corresp. to currval in array of odds {1, 3, ...}
    int start_idx;  // index in grid_local to start marking of multiples
    int k;

    currval = 3;
    curr_idx = 1;

    /* Each iteration marks all of the multiples of currval (such that the
     * multiple is >= currval^2) in the set {1, ..., n} as having factors, and
     * then finds the next value to use for currval (i.e. the next smallest
     * prime number).
     */
    while (curr_idx < rootn_setsize) {

	// Calculate corresponding index of currval^2
	start_idx = currval * currval / 2;

	/* Step through the local prime grid marking off multiples of currval as
	 * being not primes.  Note that since the local prime grid doesn't - in
	 * concept - include even numbers, so each step effectively marks off
	 * every other multiple of currval.  This however is as desired, since
	 * every second multiple of currval is an even number.
	 */
	for (k = start_idx; k < rootn_setsize; k += currval) {
	    grid_rootn[k] = YES_MARK;
	}

	/* Walk through the beginning of the prime number grid to find the index
	 * of the value that we will begin marking off multiples of in the next
	 * iteration.
	 */
	while (++curr_idx < rootn_setsize) {
	    // case: value corresp. to grid elem. not marked as having a factor
	    if (! grid_rootn[curr_idx]) {
		break;
	    }
	}

	// Translate index to its corresponding value
	currval = (2 * curr_idx) + 1;
    }

}




/* Find the odd values in the set {local_low, ..., local_high} that have factors
 * and flag the corresponding elements in the array grid_local with the value
 * YES_MARK.
 */

void fill_grid_local_v2(char *grid_local, char *grid_rootn, int rootn_setsize, 
			int local_low, int local_high, int local_setsize) {
    
    int currval;    // value which me mark multiples of in grid
    int curr_idx;   // index corresp. to currval in array of odds {1, 3, ...}
    int start_idx;  // index in grid_local to start marking of multiples
    int root_high;  // floor( sqrt(local_high) )
    int k;

    currval = 3;
    curr_idx = 1;

    root_high = (int) sqrt(local_high);

    /* Each iteration marks all of the multiples of currval (such that the
     * multiple is >= currval^2) in the local grid as having factors, and then
     * finds the next value to use for currval (i.e. the next smallest prime
     * number).
     */
    while (currval <= root_high) {

	/* Calculate the local index of the first odd-valued multiple of currval
	 * in the set that is also greater than currval^2
	 */
	start_idx = num_odd_past(currval, local_low);

	/* Step through the local prime grid marking off multiples of currval as
	 * being not primes.  Note that since the local prime grid doesn't - in
	 * concept - include even numbers, so each step effectively marks off
	 * every other multiple of currval.  This however is as desired, since
	 * every second multiple of currval is an even number.
	 */
	for (k = start_idx; k < local_setsize; k += currval) {
	    grid_local[k] = YES_MARK;
	}

	/* Walk through the beginning of the prime number grid to find the index
	 * of the value that we will begin marking off multiples of in the next
	 * iteration.
	 */
	while (++curr_idx < rootn_setsize) {
	    // case: value corresp. to grid elem. not marked as having a factor
	    if (! grid_rootn[curr_idx]) {
		break;
	    }
	}

	// Translate index to the value that the index represents
	currval = (2 * curr_idx) + 1;
    }
}




/* Find the odd values in the set {local_low, ..., local_high} that have factors
 * and flag the corresponding elements in the array grid_local with the value
 * YES_MARK.
 *
 * PRE: assumes p is an even number
 */

void fill_grid_local_v3(char *grid_local, char *grid_rootn, int rootn_setsize,
			int local_low, int local_high, int p) {
    
    int currval;    // value which me mark multiples of in grid
    int curr_idx;   // index corresp. to currval in array of odds {1, 3, ...}
    int start_idx;  // index in grid_local to start marking of multiples

    int subblock_low;        // lowest odd value in local subset
    int subblock_high;       // highest value in local subset (can be even)
    int subblock_setsize;    // number of odd values in local subset
    int root_subblock_high;  // floor( sqrt(subblock_high) )
    int subblock_offset;     // how many odd values in local set before subset
    int k;

    subblock_low = local_low;
    subblock_high = local_low + p - 1;
    subblock_setsize = p / 2;

    while (subblock_low <= local_high) {

	currval = 3;
	curr_idx = 1;

	/* case: the last subblock etends past the end of the local set.  Ajust
	 * subblock_high and subblock_setsize
	 */
	if (subblock_high > local_high) {
	    subblock_high = local_high;
	    subblock_setsize = ((subblock_high - subblock_low) / 2) + 1;
	}

	root_subblock_high = (int) sqrt(subblock_high);

	/* Each iteration marks all of the multiples of currval (such that the
	 * multiple is >= currval^2) in the local sub-grid as having factors,
	 * and then finds the next value to use for currval (i.e. the next
	 * smallest prime number).
	 */
	while (currval <= root_subblock_high) {

	    /* Calculate the local index of the first odd-valued multiple of
	     * currval in the set that is also greater than currval^2
	     */
	    start_idx = num_odd_past(currval, subblock_low);

	    /* Step through the local prime sub-grid marking off multiples of
	     * currval as being not primes.  Note that since the local prime
	     * grid doesn't - in concept - include even numbers, so each step
	     * effectively marks off every other multiple of currval.  This
	     * however is as desired, since every second multiple of currval is
	     * an even number.
	     */
	    subblock_offset = ((subblock_low - local_low) / 2);
	    for (k = start_idx; k < subblock_setsize; k += currval) {
		grid_local[k + subblock_offset] = YES_MARK;
	    }

	    /* Walk through the beginning of the prime number grid to find the
	     * index of the value that we will begin marking off multiples of in
	     * the next iteration.
	     */
	    while (++curr_idx < rootn_setsize) {
		// case: value corresp. to grid elem. not marked as having a
		// factor
		if (! grid_rootn[curr_idx]) {
		    break;
		}
	    }

	    // Translate index to the value that the index represents
	    currval = (2 * curr_idx) + 1;
	}

	/* Update subblock.  Note that we assume that p is an even-numbered
	 * value so that subblock_low is always odd
	 */
	subblock_low += p;
	subblock_high += p;	
    }
}




/* Select every rank-th prime in {3, ..., rootn} to mark multiples of off in the
 * set {rootn + 1, ..., n}, such that the multiples are larger then the square
 * of the prime
 */

void fill_grid_local_v4(char *grid_local, char *grid_rootn, int rootn_setsize,  
			int local_low, int local_setsize, int rank, int size) {
    
    int currval;    // value which me mark multiples of in grid
    int curr_idx;   // index corresp. to currval in array of odds {1, 3, ...}
    int start_idx;  // index in grid_local to start marking of multiples

    int ctr;  // **********
    int k;

    ctr = rank;
    curr_idx = 0;

    /* Each iteration marks all of the multiples of currval (such that the
     * multiple is >= currval^2) in the grid as having factors, and then finds
     * the next value to use for currval (i.e. the next smallest prime number).
     */
    while (1) {

	/* Walk through the beginning of the prime number grid to find the index
	 * of the value that we will begin marking off multiples of in the next
	 * iteration.
	 */
	while (++curr_idx < rootn_setsize) {

	    // case: value corresp. to grid elem. not marked as having a factor
	    if(! grid_rootn[curr_idx]) {

		// case: it's the rank-th turn to mark off multiples
		if (! (++ctr % size)) {
		    // exit inner loop
		    break;
		}
	    }
	} // end find next rank-th prime number index loop
	
	// case: we've iterated through the first rootn numbers; exit main loop
	if (curr_idx == rootn_setsize) {
	    break;
	}

	// Translate index to the value that the index represents
	currval = (2 * curr_idx) + 1;

	/* Calculate the local index of the first odd-valued multiple of currval
	 * in the set that is also greater than currval^2
	 */
	start_idx = num_odd_past(currval, local_low);

	/* Step through the local prime grid marking off multiples of currval as
	 * being not primes.  Note that since the local prime grid doesn't - in
	 * concept - include even numbers, so each step effectively marks off
	 * every other multiple of currval.  This however is as desired, since
	 * every second multiple of currval is an even number.
	 */
	for (k = start_idx; k < local_setsize; k += currval) {
	    grid_local[k] = YES_MARK;
	}

    } // end mark off every rank-th prime number multiples loop
}





/* Return the number of primes in the array with length len and pointed to by
 * grid, where a prime has value 0 and not prime has value 1
 */

int count_primes(char *grid, int len) {

    int k;
    int ct;

    // Count the number of primes in the local set
    ct = 0;
    for (k = 0; k < len; k++) {
	// case: grid[k] hasn't been marked as having factors
	if (!grid[k]) {
	    ct++;
	}
    }

    return ct;
}
