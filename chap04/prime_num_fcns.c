
#include <mpi.h>
#include <math.h>    // sqrt

#define TOLERANCE 0.000000000001  // 1e-12

#define FALSE    0
#define TRUE     1


/* Split the numbers from 1 to n into size nearly evenly sized sets; *startnum
 * and *stopnum are the smallest and largest values of the rank-th set
 */

void find_process_set(int rank, int size, int maxnum, int *startnum, int *stopnum) {

    /* Calculate smallest number and largest number that each process considers.
     * Note that (rank * maxnum) / size == floor(r * (m / z)) for r, m, z the
     * real-number counterparts of the integer arithmetic here.
     */
    *startnum = ((rank * maxnum) / size) + 1;
    *stopnum = ((rank + 1) * maxnum) / size;

    // case: startnum is an even number.  Advance to first odd number in set
    if (((*startnum) % 2) == 0) {
	(*startnum)++;
    }
}




/* Return TRUE if w is prime, FALSE if either (i) not prime or if (ii) it is a
 * value < 2 and consequently is undefined.
 *
 * This function code is an implementation of
 * https://en.wikipedia.org/wiki/Primality_test#Pseudocode.
 */

int check_prime(int w) {

    int sqrt_w;
    int k;

    // Prime numbers are only defined for w >= 2
    if (w <= 1) {
	return FALSE;
    }

    // case: w == 2 or w == 3; these are both primes
    if (w <= 3) {
	return TRUE;
    }
    // case: w divisible by 2 or 3.  Checks for two common cases.
    else if (((w % 2) == 0) || ((w % 3) == 0)) {
	return FALSE;
    }

    /* We already know if w is not divisible by k < 5 so we can start there.
     * After each iteration of the loop such that the inner condition failed, we
     * can be sure that w is not divisible by any integer in w, ..., w + 5.
     *
     * Note that k is always odd for every iteration.  Thus, k + 1, k + 3, and k
     * + 5 are even, and we've already checked n for evenness.  k + 4 is
     * divisible by 3 which we've already checked for, so it remains only to
     * check for divisibilty by k and k + 2.
     */
    sqrt_w = (int) sqrt(w);
    for (k = 5; k <= sqrt_w; k += 6) {
	if (((w % k) == 0) || ((w % (k + 2)) == 0)) {
	    return FALSE;
	}
    }

    // If we've made it here then no factor exists
    return TRUE;
}
