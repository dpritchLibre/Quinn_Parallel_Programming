
/* Page 112
 *
 * The gap between consecutive prime numbers 2 and 3 is only 1, while the gap
 * between consecutive primes 7 and 11 is 4.  Write a parallel program to
 * determine, for all integers less than 1,000,000, the largest gap between a
 * pair of consecutive prime numbers.
 */

/* Accepts an argument "n" for the largest value up to which we should search
 * for all consecutive odd numbers that are prime.
 */

#include <mpi.h>
#include <stdio.h>
#include "prime_num_fcns.h"
#include "parse_n.h"

#define FALSE    0
#define TRUE     1


int main(int argc, char *argv[]) {

    int size;
    int rank;

    int n;           // largest value to search for consec. odd primes
    int startnum;    // the smallest number a process considers
    int stopnum;     // the largest number a process considers

    int last_prime;  // the most recent prime found in working set
    int prime_diff;  // the length between the most recent and prev. prime
    int k;

    int local_run;   // track the local maxnumber of pairs of primes
    int global_run;  // track the overall number of pairs of primes

    local_run = global_run = 0;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    // Get the number of the processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Default value of the largest value to consider in search for consecutive
     * odd primes as given by the prompt.  If an argument for n is given as an
     * option then this value is written to n in parse_args.
     */
    n = 1e6;
    parse_args(argc, argv, &n);
    if (n < 2) {
	fprintf(stderr, "n must be >= 2\n");
	MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
    }

    /* Globally: split the numbers from 1 to n into size nearly evenly sized
     * sets.  Locally: set startnum and stopnum to be the smallest and largest
     * values of the rank-th set
     */
    find_process_set(rank, size, n, &startnum, &stopnum);

    /* Each iteration checks an odd number to see if it is prime; if so, then
     * the length of the run between prime numbers is compared to the previous
     * largest, and if so then the largest run value is updated.
     *
     * Note that the loop typically ends once a prime has been found greater
     * than the last number in the set.  This is so that a possible run that
     * starts in one set and ends in the next does not get truncated
     * prematurely.  However if we are in the last set, then we do want to
     * truncate the run which is why we test for k <= n.
     */
    last_prime = 0;
    for (k = startnum; last_prime <= stopnum && (k <= n); k += 2) {

	if (check_prime(k)) {

	    // case: first prime found in set (signaled by last_prime == 0)
	    if (! last_prime) {
		// noop
	    }
	    // case: not the first prime we've found in set; check run length
	    else {
		prime_diff = k - last_prime;
		if (prime_diff > local_run) {
		    local_run = prime_diff;
		}
	    }

	    // update value of the most recent prime found in set
	    last_prime = k;
	}
    }
		
    printf("rank %d processor:\n"
	   "The largest run between prime numbers with the first prime number\n"
	   "of the pair in the set from %d to %d (inclusive) is:  %d\n"
	   "\n",
	   rank, startnum, stopnum, local_run);

    // Collect local consecutive primes count
    MPI_Reduce(&local_run, &global_run, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    // Finalize the MPI environment.
    MPI_Finalize();

    /* The algorithm doesn't work properly for n < 5 b/c we skip past even
     * numbers and consequently we miss the run for 2 and 3.  If n == 2 then we
     * just let global_run stay at 0.
     */
    if ((n == 3) || (n == 4)) {
	global_run = 1;
    }

    // Print result
    if (rank == 0) {
	printf("------------------------------------------------\n"
	       "The largest run between prime numbers in the set\n"
	       "2 and %d (inclusive) is:  %d\n"
	       "------------------------------------------------\n"
	       "\n",
	       n, global_run);
    }

    return 0;
}

