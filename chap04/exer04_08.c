
/* Page 112
 *
 * A prime number is a positive integer evenly divisible by exactly two positive
 * integers: itself and 1.  Write a parallel program to determine, for all
 * integers less than 1,000,000, the number of times that two consecutive odd
 * integers are both prime.
 */

/* Accepts an argument "n" for the largest value up to which we should search
 * for all consecutive odd numbers that are prime.
 */

/* Note: according to http://mathworld.wolfram.com/PrimeNumber.html a prime
 * number by definition must be >= 2 contrary to what the instructions say.
 */

/* Perhaps surprisingly, this algorithm appears to work for all choices of size
 * and STOPNUM.
 *
 * When size / STOPNUM < 1 then some processes will check the same region.
 * However, the only time that a pair of primes is found is when { rank *
 * splitsize + 1 == (rank + 1) * splitsize } b/c then we check a region of
 * length 2; when the LHS > RHS we check a region of length 0 or 1 depending on
 * then LHS being even or odd (LHS < RHS can't happen).  But this equality can
 * only happen once per number, so we never do check the same pair twice.
 *
 * When size / STOPNUM >= 1 then the same pair is never checked twice, so as
 * long as all pairs are checked the algorithm is correct.
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

    int n;             // largest value to search for consec. odd primes
    int startnum;      // the smallest number a process considers
    int stopnum;       // the largest number a process considers

    int curr;  // if current odd integer is prime
    int next;  // if next odd integer is prime
    int i;

    int localct;   // track the local number of pairs of primes
    int globalct;  // track the overall number of pairs of primes

    localct = globalct = 0;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    // Get the number of the processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Default value of the largest value to consider in search for consecutive
     * odd primes as given by the prompt.  If an argument for n is given as an
     * option then this value is written to *n in parse_args.
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

    /* In the following loop we actually want to compare one odd number past
     * stopnum; this is b/c we need to see if the largest odd number in a set
     * and the smallest odd number in the next are both prime.  However we don't
     * want to do this on the last set, hence conditional for this case.
     */
    if (rank != (size - 1)) {
	stopnum += 2;
    }

    /* Each iteration checks if the pair of odd numbers (i - 2) and i are both
     * prime and if so adds 1 to localct.
     */
    curr = FALSE;
    for (i = startnum; i <= stopnum; i += 2) {
	next = check_prime(i);
	// Add 1 if both curr and next are prime, 0 otherwise
	localct += curr && next;
	// Update curr for next iteration
	curr = next;
    }

    printf("rank %d processor:\n"
	   "The number of times that consecutive odd numbers between \n"
	   "%d and %d (inclusive) are both prime is:  %d\n"
	   "\n",
	   rank, startnum, stopnum, localct);

    // Collect local consecutive primes count
    MPI_Reduce(&localct, &globalct, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Finalize the MPI environment.
    MPI_Finalize();

    // Print result
    if (rank == 0) {
	printf("--------------------------------------------------------\n"
	       "The number of times that consecutive odd numbers between\n"
	       "2 and %d (inclusive) are both prime is:  %d\n"
	       "--------------------------------------------------------\n"
	       "\n",
	       n, globalct);
    }

    return 0;
}
