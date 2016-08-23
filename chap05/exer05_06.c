
/* Page 135
 *
 * Modify the parallel Sieve of Eratosthenes program presented in the text to
 * incorporate the first improvement described in Section 5.9: it should not set
 * aside memory for even integers.
 */

/* The reason that the algorithm works is the following.  Clearly it is
 * sufficient to mark all multiples of 2, 3, ..., floor( sqrt(n) ), as not
 * prime, and then what remains is prime.  This is what the Sieve of
 * Eratosthenes essentially does, however it eliminates some redundancies by
 * only marking off the multiples of each k starting at k^2.  The reason that
 * this shortcut works is because if a number is a multiple of k smaller than
 * k^2, then it has a value given by k * m for some integer m with m < k, and we
 * have already tested for multiples of m.
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

    int local_low;      // lowest odd value in local set
    int local_high;     // highest value in local set (can be even)
    int local_setsize;  // number of odd values in local set

    char *grid_local;   // memory used to track if values in set have factors

    int nprime_local;   // number of prime numbers in local set
    int nprime_global;  // number of prime numbers in {2, 3, ..., n}
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

    /* Calculate the smallest odd number in the rank-th set, the largest number
     * in the rank-th set, and the number of odd values in the set, and store
     * these values in *local_low, *local_high, and *local_setsize, respectively.
     */
    local_set_params(1, n, rank, size, &local_low, &local_high, &local_setsize);
    /* Algorithm is not valid unless the 0-th process includes all odd numbers
     * from 1 to sqrt(n) in its set
     */
    if (!rank && (local_high < rootn)) {
	fprintf(stderr, "the ratio of n / size is too low\n");
	MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
    }

    /* Allocate memory for prime number grids and set each element in each grid to 
     * not having a factor state
     */
    initialize_grid(&grid_local, local_setsize);

    /* 
     */
    fill_grid_v1(grid_local, rootn, local_low, local_setsize, rank);

    // Count the number of primes found in each set
    nprime_local = count_primes(grid_local, local_setsize);

    // Free data
    free(grid_local);
    
    // Find the sum of the primes found in each process
    MPI_Reduce(&nprime_local, &nprime_global, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

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
	printf("%d primes are less than or equal to %d\n"
	       "Total elapsed time: %10.6f\n"
	       "\n",
	       nprime_global, n, elapsed);
    }

    return 0;
}
