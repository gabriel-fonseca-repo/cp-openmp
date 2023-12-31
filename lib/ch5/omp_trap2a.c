/* File:    omp_trap2a.c
 * Purpose: Estimate definite integral (or area under curve) using trapezoidal
 *          rule.  This version uses a hand-coded reduction after the function
 *          call.
 *
 * Input:   a, b, n
 * Output:  estimate of integral from a to b of f(x)
 *          using n trapezoids.
 *
 * Compile: gcc -g -Wall -fopenmp -o omp_trap2a omp_trap2a.c -lm
 * Usage:   ./omp_trap2a <number of threads>
 *
 * Notes:
 *   1.  The function f(x) is hardwired.
 *   2.  This version assumes that n is evenly divisible by the
 *       number of threads
 * IPP:  Section 5.4 (p. 222)
 */

#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void como_usar(char* prog_name);
double f(double x); /* Function we're integrating */
double Local_trap(double a, double b, int n);

int main(int argc, char* argv[]) {
    double global_result; /* Store result in global_result */
    double a, b;          /* Left and right endpoints      */
    int n;                /* Total number of trapezoids    */
    int thread_count;
    double start_time, end_time;

    thread_count = strtol(argv[1], NULL, 10);  // convert string to long int
    a = strtol(argv[2], NULL, 10);
    b = strtol(argv[3], NULL, 10);
    n = strtol(argv[4], NULL, 10);
    if (n % thread_count != 0) como_usar(argv[0]);

    start_time = omp_get_wtime();
    global_result = 0.0;
#pragma omp parallel num_threads(thread_count)
    {
        double my_result = 0.0;
        my_result += Local_trap(a, b, n);
#pragma omp critical
        global_result += my_result;
    }
    end_time = omp_get_wtime();

    printf("With %d threads\n", thread_count);
    printf("With n = %d trapezoids, our estimate \n", n);
    printf("of the integral from %f to %f = %.14e \n", a, b, global_result);
    printf("The time taken by the parallel: %lf \n", end_time - start_time);
    printf("\n");
    return 0;
} /* main */

/*--------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     Print command line for function and terminate
 * In arg:      prog_name
 */
void como_usar(char* prog_name) {
    fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
    fprintf(stderr, "   number of trapezoids must be evenly divisible by\n");
    fprintf(stderr, "   number of threads\n");
    exit(0);
} /* Usage */

/*------------------------------------------------------------------
 * Function:    f
 * Purpose:     Compute value of function to be integrated
 * Input arg:   x
 * Return val:  f(x)
 */
double f(double x) {
    double return_val;

    return_val = x * x;
    return return_val;
} /* f */

/*------------------------------------------------------------------
 * Function:    Local_trap
 * Purpose:     Use trapezoidal rule to estimate part of a definite
 *              integral
 * Input args:
 *    a: left endpoint
 *    b: right endpoint
 *    n: number of trapezoids
 * Return val:  estimate of integral from local_a to local_b
 *
 * Note:        return value should be added in to an OpenMP
 *              reduction variable to get estimate of entire
 *              integral
 */
double Local_trap(double a, double b, int n) {
    double h, x, my_result;
    double local_a, local_b;
    int i, local_n;
    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();

    h = (b - a) / n;
    local_n = n / thread_count;
    local_a = a + my_rank * local_n * h;
    local_b = local_a + local_n * h;
    my_result = (f(local_a) + f(local_b)) / 2.0;
    for (i = 1; i <= local_n - 1; i++) {
        x = local_a + i * h;
        my_result += f(x);
    }
    my_result = my_result * h;

    return my_result;
} /* Trap */