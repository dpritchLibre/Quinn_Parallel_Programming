
/* Simpson's rule is a better numerical integration algorithm than the rectangle
 * rule because it converges more quickly.  Suppose we want to compute int_a^b
 * f(x) dx.  We divide the interval [a, b] into n subintervals, were n in even.
 * Let x_i denote the end of the i-th interval, for 1 <= i <= , and let x_0
 * denote the beginning of the first interval.  According to Simpson's rule:
 *
 *     \int_a^b f(x) dx \approx \frac{1}{3n} \left[ f(x_0) - f(x_n) + 
 *         \sum_{i=1}^{n/2} (4 f(x_{2i} - 1) + 2 f(x_{2i})) \right]
 *
 * Write a parallel program to compute the value of pi using Simpson's rule.
 */

/* Accepts an argument "n" for the number of intervals that we should
 * approximate the function with.
 */

#include <mpi.h>
#include <stdio.h>
#include "parse_n.h"

#define LBND 0  // lower bound of definite integral
#define UBND 1  // upper bound of definite integral

double eval_fcn(double x);


int main(int argc, char *argv[]) {

    int rank;
    int size;
    
    double interval_len;  // size of interval
    int n;                // number of intervals to divide domain into

    double local_area;   // local sum of process function evaluations
    double global_area;  // global sum of Simpson's rule terms

    int k;
    int halfn;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    // Get the number of the processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Default value of the number of intervals to break the integral into.  If
     * an argument for n is given as an option then this value is written to *n
     * in parse_args.
     */
    n = 50;
    parse_args(argc, argv, &n);

    /* Calculate the interval length and the length of the midpoint within the
     * interval.  Note that we do not have to consider underflow b/c n has to be
     * representable as an int.
     */
    interval_len = 1.0 / ((double) n);

    /* Evaluate every size-th pair of function evaluations in the sum portion of
     * Simpson's rule.  The first term is 4*f(x_{2k - 1}) and the second term is
     * 2*f(x_{2k}).
     */
    local_area = 0;
    halfn = n / 2;
    for (k = (rank + 1); k <= halfn; k += size) {
	local_area += 4 * eval_fcn(((2 * k) - 1) * interval_len);
	local_area += 2 * eval_fcn((2 * k) * interval_len);
    }

    // Sum the unnormalized processes areas
    MPI_Reduce(&local_area, &global_area, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    // Finalize the MPI environment.
    MPI_Finalize();

    // Add first and last term and normalize
    global_area += eval_fcn(LBND) - eval_fcn(UBND);
    global_area /= 3 * n;
    
    // Print result
    if (rank == 0) {
	printf("\n"
	       "The value obtained by Simpson's rule is:  %.14f\n"
	       "The leading digits of pi is:              3.14159265358979\n"
	       "\n",
	       global_area);
    }
    
    return 0;
}


double eval_fcn(double x) {
    return 4 / (1 + (x * x));
}

