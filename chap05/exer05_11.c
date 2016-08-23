
/* Page 136
 *
 * The simplest harmonic progression is
 *
 *     1/1, 1/2, 1/3, ...
 *
 * Let S_n = \sum_{i=1}^n.  Write a parallel program that computes these sums to
 * arbitrary precision after the decimal point.  For example, S_7 =
 * 2.592857142857 to 12 digits of precision after the decimal point.  Process 0
 * should query the user for two parameters, n and d, and broadcast these
 * parameters to other processes.
 */

#include <mpi.h>
#include <stdio.h>
#include <float.h>

#include "parse_args.h"


int main(int argc, char *argv[]) {

    int rank;  // process rank
    int size;  // number of processes
    
    int n;  // variable n in sum_{k=1}^n 1 / k
    int d;  // precision with which to print S_n
    int k;

    double harm_local;   // store the local portion of S_n
    double harm_global;  // store the entire value of S_n

    harm_local = 0;
    harm_global = 0;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    // Get the number of the processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Process 0 looks for command-line arguments for n and d and updates their
     * values if present.  These values are then broadcast to the remaining
     * processes.
     *
     * It is surely better to just let each process read the arguments for
     * themselves, but for the sake of the exercise we will do as prompted.
     */
    if (!rank) {
	n = 1000;
	d = 15;
    	parse_args(argc, argv, &d, &n, NULL);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&d, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Compute the local share of S_n
    for (k = rank + 1; k <= n; k += size) {
	harm_local += 1.0 / k;
    }
    
    // Find the sum of the primes found in each process
    MPI_Reduce(&harm_local, &harm_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Finalize the MPI environment
    MPI_Finalize();

    if (!rank) {
	printf("\n"
	       "The value of S_n for n = %d and printed to %d digits of\n"
	       "precision after the decimal point is:\n"
	       "\n"
	       "    %.*f\n"
	       "\n",
	       n, d, d, harm_global);
    }	       

    return 0;
}
