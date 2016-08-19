
/* Page 112
 *
 * Write a parallel program that computes the sum 1 + 2 + ... + p in the
 * following manner: Each process i assigns the value i + 1 to an integer, and
 * then the processes perform a sum reduction of these values.  Process 0 should
 * print the result of the reduction.  As a way of double-checking the result,
 * process 0 should also compute and print the value p(p + 1) / 2.
 */

/* Accepts an argument "p" for the value up to which we should sum to.
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>

void parse_args(int argc, char* argv[], int *p);


int main(int argc, char *argv[]) {

    int size;
    int rank;
    int local_sum;
    int global_sum;
    int k;
    int p;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of the processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Arbitrary choice of p set as an example.  If an argument for p is given
     * as an option then this value is written to p in parse_args.
     */
    p = 1000;
    parse_args(argc, argv, &p);

    // Sum every size-th integer <= p
    local_sum = 0;
    for (k = rank + 1; k <= p; k += size) {
	local_sum += k;
    }

    // Collect local sums
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Finalize the MPI environment.
    MPI_Finalize();

    // Print result
    if (rank == 0) {
	printf("\n"
	       "The value of p was specified as:  %d\n"
	       "The value obtained by summing 1 + 2 + ... + p is:  %d\n"
	       "The value of p(p + 1) / 2 is:  %d\n"
	       "\n",
	       p, global_sum, p * (p + 1) / 2);
    }

    return 0;
}


/* Parse user parameter specifications and set the appropriate variable values.
 * Input of arguments is allowed following the usual UNIX conventions.
 */

void parse_args(int argc, char* argv[], int *p) {

    int opt;        // argument type info
    char* endptr;   // point to next char after int read (should point to '\0')

    // To distinguish success / failure after a call to strtol or strtod
    errno = 0;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
	switch (opt) {
	case 'p':
	    *p = strtol(optarg, &endptr, 10);
	    if (*endptr != '\0') {
		fprintf(stderr, "Invalid argument for p\n");
		MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
	    }
	    else if (errno != 0) {
		fprintf(stderr, "Underflow / overflow for p\n");
		MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
	    }
	    else if (*p < 1) {
		fprintf(stderr, "p must be >= 1\n");
		MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
	    }
	    break;
	case '?':
	    // Note: error message automatically written to stderr by getopt
	    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
	case ':':
	    // Note: error message automatically written to stderr by getopt
	    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
	}
    }
}
