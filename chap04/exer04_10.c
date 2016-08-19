
/* Page 112
 *
 * A small college wishes to assign unique identification numbers to all of its
 * present and furture students.  The administration is thinking of using a
 * six-digit identifier, but is not sure that there will be enough combinations,
 * given various constraints that have been placed on what is considered to be
 * an "acceptable" identifier.  Write a parallel program to count the number of
 * different six digit combinations of the numerals 0-9, given these
 * constraints:
 *
 *    - The first digit may not be a 0
 *    - Two consecutive digits may not be the same
 *    - The sum of the digits may not be 7, 11, or 13
 */

#include <mpi.h>
#include <stdio.h>

#define NDIGITS       6  // number of digits in id
#define MINNUM   101010  // smallest 6-digit number satisfying req's 
#define MAXNUM   989898  // largest 6-digit number satisfying req's

#define FALSE 0
#define TRUE  1

int check_satisfy(int val);


int main(int argc, char *argv[]) {
    
    int size;
    int rank;
    int localct;
    int globalct;
    int k;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    // Get the number of the processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Each iteration checks if the value satisfies the criteria and if so adds
     * 1 to localct, or adds 0 otherwise.
     */
    localct = 0;
    for (k = rank + MINNUM; k <= MAXNUM; k += size) {
	localct += check_satisfy(k);
    }

    // Collect local counts
    MPI_Reduce(&localct, &globalct, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Finalize the MPI environment.
    MPI_Finalize();

    // Print result
    if (rank == 0) {
    	printf("\n"
    	       "The number of id combinations satisfying the criteria was:  %d\n"
	       "\n",
    	       globalct);
    }

    return 0;
}


/* Check that the last six digits of val neither (i) have consecutive digits of
 * the same value, nor (ii) the sum of the digits isn't 7, 11, or 13.
 *
 * Note that we are not checking that the first digit isn't a 1.
 */ 

int check_satisfy(int val) {

    int digits[6];  // store each digit in val
    int digsum;     // sum of the digits
    int i;

    /* Each iteration finds the value of the last digit of val and then strips
     * val of that digit.  Note that this places the values stored in digits in
     * the opposite order of the original value, however this does not matter
     * for our problem.
     */
    digits[0] = val % 10;
    val /= 10;
    for (i = 1; i < NDIGITS; i++) {
	digits[i] = val % 10;
	val /= 10;

	if (digits[i - 1] == digits[i]) {
	    return FALSE;
	}
    }

    /* If we've made it this far then no two consecutive digits are equal.
     * Check the sum of the digits.
     */
    digsum = 0;
    for (i = 0; i < NDIGITS; i++) {
	digsum += digits[i];
    }
    if ((digsum == 7) || (digsum == 11) || (digsum == 13)) {
	return FALSE;
    }
 
    // If we've made it this far then we've passed every criterion
    return TRUE;
}
