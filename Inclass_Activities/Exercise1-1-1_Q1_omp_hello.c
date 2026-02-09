#include <stdio.h>
#include <stdlib.h>
#ifdef _OPENMP
#include <omp.h> //NOTE: Conditional inclusion ofthe OpenMP library
#endif

void Hello();

int main(int argc, char* argv[]) {
        int thread_count = strtol(argv[1], NULL, 10);

#  pragma omp parallel num_threads(thread_count) // NOTE: OpenMP directives will be ignored by a non-compatible compiler
        Hello();
        return 0;
}  /* main */

// NOTE: Conditional execution of the OpenMP functions
void Hello(void) {
        #ifdef _OPENMP
        int my_rank = omp_get_thread_num();
        int actual_thread_count = omp_get_num_threads();
        #else
        int my_rank = 0;
        int actual_thread_count = 1;
        #endif

        printf("Hello from thread %d of %d\n", my_rank, actual_thread_count);
        
}  /* Hello */
