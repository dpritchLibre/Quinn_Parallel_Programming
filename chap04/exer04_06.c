
/* Page 112
 *
 * Write a parallel variant of Kenighan and Ritchie's classic "hello, world"
 * program.  Each proccess should print a message of the form
 *
 *    hello, world, from process <i>
 *
 * where <i> is its rank.
 */

#include <mpi.h>
#include <stdio.h>


int main(int argc, char *argv[]) {

    int size;
    int rank;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of the processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Print off a hello world message
    printf("hello, world, from process %d out of %d processes\n",
           rank, size);

    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}
