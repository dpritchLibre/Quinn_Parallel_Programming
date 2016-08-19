
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


/* Parse user parameter specifications for a positive integer n and write value
 * to *n.
 */

void parse_args(int argc, char* argv[], int *n) {

    int opt;        // argument type info
    char* endptr;   // point to next char after int read (should point to '\0')

    // To distinguish success / failure after a call to strtol or strtod
    errno = 0;

    while ((opt = getopt(argc, argv, "n:")) != -1) {
	switch (opt) {
	case 'n':
	    *n = strtol(optarg, &endptr, 10);
	    if (*endptr != '\0') {
		fprintf(stderr, "Invalid argument for n\n");
		MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
	    }
	    else if (errno != 0) {
		fprintf(stderr, "Underflow / overflow for n\n");
		MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
	    }
	    else if (*n < 1) {
		fprintf(stderr, "n must be >= 1\n");
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
