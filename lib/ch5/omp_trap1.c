/*
 * Input:   a, b, n
 * Output:  estimate of integral from a to b of f(x)
 *          using n trapezoids.
 * Compile: gcc -g -Wall -fopenmp -o omp_trap1 omp_trap1.c
 * Usage:   ./omp_trap1 <number of threads>
 */
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void como_usar(char* prog_name);
double f(double x); /* Function we're integrating */
double Local_Trap(double a, double b, int n);

int main(int argc, char* argv[]) {
    double global_result = 0.0; /* Store result in global_result */
    double a, b;                /* Left and right endpoints      */
    int n;                      /* Total number of trapezoids    */
    int thread_count;
    double start_time, finish_time;

    thread_count = strtol(argv[1], NULL, 10);  // convert string to long int
    a = strtol(argv[2], NULL, 10);
    b = strtol(argv[3], NULL, 10);
    n = strtol(argv[4], NULL, 10);
    if (n % thread_count != 0) como_usar(argv[0]);

    /*The following is the modified part*/
    start_time = omp_get_wtime();
#pragma omp parallel num_threads(thread_count)
    {
#pragma omp critical
        global_result += Local_Trap(a, b, n);
    }
    finish_time = omp_get_wtime();

    printf("With %d threads\n", thread_count);
    printf("With n = %d trapezoids, our estimate \n", n);
    printf("of the integral from %f to %f = %.14e \n", a, b, global_result);
    printf("The time taken by the parallel: %lf \n", finish_time - start_time);
    printf("\n");
    return 0;
} /* main */

void como_usar(char* prog_name) {
    fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
    fprintf(stderr, "   number of trapezoids must be evenly divisible by\n");
    fprintf(stderr, "   number of threads\n");
    exit(0);
} /* Usage */

double f(double x) {
    double return_val;

    return_val = x * x;
    return return_val;
} /* f */

/*------------------------------------------------------------------
 * Function:    Trap
 * Purpose:     Use trapezoidal rule to estimate definite integral
 * Input args:
 *    a: left endpoint
 *    b: right endpoint
 *    n: number of trapezoids
 * Output arg:
 *    integral:  estimate of integral from a to b of f(x)
 */
double Local_Trap(double a, double b, int n) {
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