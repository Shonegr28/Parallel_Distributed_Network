#include <stdio.h>
#include <stdlib.h>
#ifdef _OPENMP
#include <omp.h> //NOTE: Conditional inclusion ofthe OpenMP library
#endif 
void Hello(void);
int main(int argc, char* argv[]) {
    printf("Start!\n");
#  pragma omp parallel num_threads(2)
    Hello();
    printf("Goodbye!\n");
    return 0;
}  /* main */

void Hello(void) {
   int my_rank = omp_get_thread_num();
   int thread_count = omp_get_num_threads();
   printf("Hello from thread %d of %d\n", my_rank, thread_count);
}  /* Hello */
