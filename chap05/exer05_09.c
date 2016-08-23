
/* Pages 135-136
 *
 * All of the parallel sieve algorithms developed in this chapter are the result
 * of a domain decomposition of the original algorithm.  Write a parallel Sieve
 * of Eratosthenes program based upon a functional decomposition of the
 * algorithm.  Suppose there are p proesses finding primes up to n.  In the
 * first step each process independently identifies primes up to sqrt(n).  In
 * step two each process sieves the list of integers with (1 / p)-th of the
 * primes between 2 and sqrt(n).  During the third step the processes OR_reduce
 * their arrays into a single array held by process 0.  In the last step process
 * 0 counts the unmarked elements of the array and prints the prime number
 * count.
 *
 * For example, suppose three processes are cooperating to find primes up to
 * 1000.  Each process allocates an array of 999 elements, representing the
 * integers 2 through 1000.  Each process identifies the primes less than or
 * equal to sqrt(1000): 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31.  Process 0
 * sieves its array with the primes 2, 7, 17, and 29; process 1 sieves its array
 * with the primes 3, 11, 19, and 31; and process 2 sieves its array with the
 * primes 5, 13, and 23.
 */

/* Accepts an argument n for the set {2, 3, ..., n} in which we search for prime
 * numbers.
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sieve_helper.h"
#include "parse_args.h"


int main(int argc, char *argv[]) {

    int rank;           // process rank
    int size;           // number of processes

    int n;              // gives the set {2, 3, ..., n} to search for primes
    int rootn;          // floor( sqrt(n) )
    int rootn_setsize;  // number of odd values in {1, ..., rootn}

    int local_low;      // lowest odd value in local set
    int local_high;     // highest value in local set (can be even)
    int local_setsize;  // number of odd values in local set

    char *grid_rootn;   // track if odd vals in {1, ..., rootn} have factors
    char *grid_local;   // track if odd vals in local set have factors

    int nprime_rootn;   // number of prime numbers in {2, ..., rootn}
    int nprime_local;   // number of prime numbers in local set
    double elapsed;     // parallel execution time

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Start the timer
    MPI_Barrier(MPI_COMM_WORLD);
    elapsed = -MPI_Wtime();

    // Get the number of the processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Default value of n is set to 1e6; if an argument for n is passed in
     * through the command line then n will be set to this value by parse_args
     */
    n = 1e6;
    parse_args(argc, argv, NULL, &n, NULL);
    rootn = sqrt(n);
    rootn_setsize = (rootn + 1) / 2;

    /* Calculate the smallest odd number in the rank-th set, the largest number
     * in the rank-th set, and the number of odd values in the set, and store
     * these values in *low_value, *high_value, and *set_size, respectively.
     */
    local_set_params(rootn + 1, n, 0, 1, &local_low, &local_high, &local_setsize);

    /* Allocate memory for prime number grids and set each element in each grid to 
     * not having a factor state
     */
    initialize_grid(&grid_rootn, rootn_setsize);
    initialize_grid(&grid_local, local_setsize);

    /* Select every rank-th prime in {3, ..., rootn} to mark multiples of off in the
     * set {rootn + 1, ..., n}, such that the multiples are larger then the square
     * of the prime
     */
    fill_grid_rootn(grid_rootn, rootn, rootn_setsize);

    /* Walk through prime number grid from {rootn + 1, ..., n} and mark
     * multiples of every rank-th prime in {3, ..., rootn} as having a factor
     */
    fill_grid_local_v4(grid_local, grid_rootn, rootn_setsize, local_low, 
		       local_setsize, rank, size);

    /* OR each of the array elements; as a result every element for which a
     * factor was found by at least one process will be marked as such
     */
    if (!rank) {
    	MPI_Reduce(MPI_IN_PLACE, grid_local, local_setsize, MPI_CHAR, MPI_LOR, 0, MPI_COMM_WORLD);
    }
    else {
    	MPI_Reduce(grid_local, NULL, local_setsize, MPI_CHAR, MPI_LOR, 0, MPI_COMM_WORLD);
    }

    // Count the number of primes
    if (!rank) {
	nprime_rootn = count_primes(grid_rootn, rootn_setsize);
	nprime_local = count_primes(grid_local, local_setsize);
    }

    // Free data
    free(grid_rootn);
    free(grid_local);

    // Stop the timer
    elapsed += MPI_Wtime();

    // Finalize the MPI environment
    MPI_Finalize();

    // Print the global number of primes results
    if (!rank) {

	printf("\n"
	       "The number of primes in the set from 2 to %d (inclusive) is %d\n"
	       "\n",
	       rootn, nprime_rootn);

	printf("The number of primes in the set from %d to %d (inclusive) is %d\n"
	       "\n",
	       local_low, local_high, nprime_local);
	       
	printf("%d primes are less than or equal to %d\n"
	       "Total elapsed time: %10.6f\n"
	       "\n",
	       nprime_rootn + nprime_local, n, elapsed);
    }

    return 0;
}

