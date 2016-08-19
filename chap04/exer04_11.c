
/* Pages 112-113
 *
 * The value of the definite integral
 *
 *     int_0^1 \frac{ 4 }{ 1 + x^2 } dx
 *
 * is pi.  We can use numerical integration to compute pi by approximating the
 * area under the curve.  A simple way to do this is called the rectangle rule.
 * We divide the interval [0, 1] into k sub intervals of equal size.  We find the
 * height of the curve at the midpoint of each of these subintervals.  With
 * these heights we can construct k rectangles.  The area of the rectangles
 * approximates the area under the curve.  As k increases the accuracy of the
 * estimate also increases.  Write a parallel program to compute pi using the
 * rectangle rule with 1,000,000 intervals.
 */

/* Accepts an argument "n" for the number of intervals that we should
 * approximate the function with.
 */

#include <mpi.h>
#include <stdio.h>
#include "parse_n.h"

double eval_fcn(double x);


int main(int argc, char *argv[]) {

    int rank;
    int size;
    
    double interval_len;  // size of each rectangle width
    double midpoint_len;  // 1/2 rect. width (i.e. where fcn is evaluated)
    int n;                // number of rectangles to divide integral into

    double local_area_unnorm;  // unnormalized sum of process rect. areas
    double global_area;        // global sum of rectangle areas

    int k;

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
    n = 1e6;
    parse_args(argc, argv, &n);

    /* Calculate the interval length and the length of the midpoint within the
     * interval.  Note that we do not have to consider underflow b/c n has to be
     * representable as an int.
     */
    interval_len = 1.0 / ((double) n);
    midpoint_len = interval_len / 2.0;

    /* Evaluates every size-th interval and adds the unnormalized area to
     * local_area_unnorm.  It is unnormalized in the sense that each rectangle
     * has area (end - start) * height, whereas what we need is ((end - start) /
     * n) * height.  However we sum the unnormalized terms and normalize at the
     * end.  Note: for this problem overflow is not an issue.
     */
    local_area_unnorm = 0;
    for (k = rank; k < n; k += size) {
	local_area_unnorm += eval_fcn((k * interval_len) + midpoint_len);
    }

    // Sum the unnormalized processes areas
    MPI_Reduce(&local_area_unnorm, &global_area, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    // Finalize the MPI environment.
    MPI_Finalize();

    // Normalize global area
    global_area /= n;
    
    // Print result
    if (rank == 0) {
	printf("\n"
	       "The value from summing the rectangles is:  %.14f\n"
	       "The leading digits of pi is:               3.14159265358979\n"
	       "\n",
	       global_area);
    }
    
    return 0;
}


double eval_fcn(double x) {
    return 4 / (1 + (x * x));
}

