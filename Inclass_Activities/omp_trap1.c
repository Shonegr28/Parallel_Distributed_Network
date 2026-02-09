/* File:    omp_grid_min.c
 * Purpose: Parallel grid search to find the MINIMUM value of f(x)
 *          within a user-defined range [a, b] using step size h.
 *
 * Input:   a, b, h
 * Output:  minimum f(x) among grid points between a and b
 *
 * Compile: gcc -g -Wall -fopenmp -o omp_grid_min omp_grid_min.c -lm
 * Usage:   ./omp_grid_min <number of threads>
 *
 * Based on: omp_trap1.c
 * Key slide fix (Exercise 1.1.2):
 *   The critical section must protect the IF statement too,
 *   not just the assignment (Answer 1 slide). :contentReference[oaicite:0]{index=0}
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef _OPENMP
#include <omp.h> //NOTE: Conditional inclusion ofthe OpenMP library
#endif

void Usage(char* prog_name);

/* same f(x) as omp_trap1.c *
double f(double x);

/* CHANGE: Trap -> Find_Minimum (grid search min instead of integral) */
void Find_Minimum(double a, double b, int n, double* global_min_p);

int main(int argc, char* argv[]) {
   double a, b;              /* left and right endpoints */
   double h;                 /* CHANGE: step size instead of trapezoids width */
   int    n;                 /* CHANGE: number of grid intervals (derived) */
   int    thread_count;
   double global_min;        /* CHANGE: global minimum (instead of sum) */

   if (argc != 2) Usage(argv[0]);
   thread_count = (int) strtol(argv[1], NULL, 10);

   /* CHANGE: input is now a, b, h (NOT a, b, n) */
   printf("Enter a, b, and h\n");
   scanf("%lf %lf %lf", &a, &b, &h);

   if (h <= 0 || b < a) Usage(argv[0]);

   /* CHANGE: number of X values is (b-a)/h (from prompt),
      we use n as number of intervals so h = (b-a)/n, same style as slides.
      This makes x = a + i*h for i=0..n.
   */
   n = (int) ((b - a) / h);     /* n intervals => (n+1) grid points */

   if (n <= 0) Usage(argv[0]);

   /* Keep the same assumption as omp_trap1.c: n divisible by thread_count */
   if (n % thread_count != 0) Usage(argv[0]);

   /* CHANGE: initialize global_min to a valid value */
   global_min = f(a);

#  pragma omp parallel num_threads(thread_count)
   Find_Minimum(a, b, n, &global_min);

   printf("Grid search minimum in [%f, %f] with h = %f\n", a, b, h);
   printf("min f(x) = %.14e\n", global_min);

   return 0;
}  /* main */

void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
   fprintf(stderr, "Input: a b h (h>0, b>=a)\n");
   fprintf(stderr, "Also requires: n = (b-a)/h is divisible by thread count\n");
   exit(0);
}

double f(double x) {
   return x*x;
}

/*------------------------------------------------------------------
 * Function:    Find_Minimum
 * Purpose:     Find minimum f(x) over assigned subinterval/grid points.
 *
 * This follows the Exercise 1.1.2 “Answer 1” fix:
 *   #pragma omp critical must wrap BOTH:
 *     - the if condition check
 *     - the assignment
 * Otherwise, the global min can be overwritten incorrectly. :contentReference[oaicite:1]{index=1}
 */
void Find_Minimum(double a, double b, int n, double* global_min_p) {
   double h, x, y, my_min;
   double local_a, local_b;
   int i, local_n;

   int my_rank = omp_get_thread_num();
   int thread_count = omp_get_num_threads();

   /* Same structure as Trap() */
   h = (b - a) / n;
   local_n = n / thread_count;
   local_a = a + my_rank * local_n * h;
   local_b = local_a + local_n * h;

   /* CHANGE: initialize local minimum using starting point */
   my_min = f(local_a);

   /* CHANGE: scan points and update local minimum (instead of summing) */
   for (i = 1; i <= local_n; i++) {    /* include local_b */
      x = local_a + i * h;
      y = f(x);
      if (my_min > y) my_min = y;
   }

   /* CHANGE (IMPORTANT FIX from slides):
      Critical must protect the IF statement as well (Answer 1). */
#  pragma omp critical
   {
      if (*global_min_p > my_min)
         *global_min_p = my_min;
   }
}  /* Find_Minimum */
