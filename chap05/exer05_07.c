
/* Page 135
 *
 * Modify the parallel Sieve of Eratosthenes program presented in the text to
 * incorporate the first two improvement described in Section 5.9.  Your program
 * should not set aside memory for even integers, and each process should use
 * the sequential Sieve of Eratosthenes algorithm on a seperate array to find
 * all primes between 3 and floor( sqrt(n) ).  With this information, the call
 * to MPI_Bcast can be eliminated.
 */

/* Accepts an argument n for the set {2, 3, ..., n} in which we search for prime
 * numbers.
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sieve-helper.h"
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
    int nprime_global;  // number of prime numbers in {2, ..., n}
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
    parse_args(argc, argv, &n, NULL);
    rootn = sqrt(n);
    rootn_setsize = (rootn + 1) / 2;
    
    /* case: at least 2 values for every set; enough to ensure that any given odd value
     * is only included in 1 set
     */
    if ((n - rootn) < (2 * size)) {
    	fprintf(stderr, "the ratio of n / size is too low\n");
    	MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
    }

    /* Calculate the smallest odd number in the rank-th set, the largest number
     * in the rank-th set, and the number of odd values in the set, and store
     * these values in *low_value, *high_value, and *set_size, respectively.
     */
    local_set_params(rootn + 1, n, rank, size, &local_low, &local_high, &local_setsize);

    /* Allocate memory for prime number grids and set each element in each grid to 
     * not having a factor state
     */
    initialize_grid(&grid_rootn, rootn_setsize);
    initialize_grid(&grid_local, local_setsize);

    /* Walk through prime number grid from {1, ..., rootn} and mark elements
     * for which a factor is found
     */
    fill_grid_rootn(grid_rootn, rootn, rootn_setsize);

    /* Walk through prime number grid from {local_low, ..., local_high} and mark
     * elements for which a factor is found
     */
    fill_grid_local_v2(grid_local, grid_rootn, rootn_setsize, 
		       local_low, local_high, local_setsize);

    // Count the number of primes found in each set
    nprime_local = count_primes(grid_local, local_setsize);
    nprime_rootn = (rank ? 0 : count_primes(grid_rootn, rootn_setsize));

    // Find the sum of the primes found in each process
    MPI_Reduce(&nprime_local, &nprime_global, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    // Add in the primes found in first rootn numbers (correct only for rank 0)
    nprime_global += nprime_rootn;

    // Free data
    free(grid_local);
    free(grid_rootn);

    // Print the number of primes found in the local set
    printf("The number of primes in the set from %d to %d (inclusive) is %d\n"
	   "\n",
	   local_low, local_high, nprime_local);

    // Stop the timer
    elapsed += MPI_Wtime();

    // Finalize the MPI environment
    MPI_Finalize();

    // Print the global number of primes results
    if (!rank) {

	fflush(stdout);
	printf("The number of primes in the set from 2 to %d (inclusive) is %d\n"
	       "\n",
	       rootn, nprime_rootn);

	printf("%d primes are less than or equal to %d\n"
	       "Total elapsed time: %10.6f\n"
	       "\n",
	       nprime_global, n, elapsed);
    }

    return 0;
}
