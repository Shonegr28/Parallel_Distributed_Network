#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define DEBUG 0

/* ----------- Project 2 - Problem 2.1 - Max of A*B (No Full C Stored) -----------
*/ // -------------------------------------------------------------------------- //

int main(int argc, char* argv[])
{
    // Catch console errors
    if (argc != 10)
    {
        printf("USE LIKE THIS: parallel_mult_mat_mat file_A.csv n_row_A n_col_A file_B.csv n_row_B n_col_B result_matrix.csv time.csv num_threads \n");
        return EXIT_FAILURE;
    }

    // Get the input files
    FILE* inputMatrix1 = fopen(argv[1], "r");
    FILE* inputMatrix2 = fopen(argv[4], "r");

    char* p1;
    char* p2;

    // Get matrix 1's dims
    int n_row1 = strtol(argv[2], &p1, 10);
    int n_col1 = strtol(argv[3], &p2, 10);

    // Get matrix 2's dims
    int n_row2 = strtol(argv[5], &p1, 10);
    int n_col2 = strtol(argv[6], &p2, 10);

    // Get num threads
    int thread_count = strtol(argv[9], NULL, 10);

    // Get output files
    FILE* outputFile = fopen(argv[7], "w");
    FILE* outputTime = fopen(argv[8], "w");


    // TODO: malloc the two input matrices and the output matrix
    char line[10240];
    // Please use long int as the variable type
    //// represent the entire input matrix 1 stores as a 1-dimensional array in row-major order
    long int *matrix1 = (long int *)malloc(n_row1 * n_col1 * sizeof(long int)); // malloc here requests a block of memory of specified size from the heap at runtime and returns a pointer to the start of that memory
    // represent the entire input matrix 2 stores as a 1-dimensional array in row-major order
    long int *matrix2 = (long int *)malloc(n_row2 * n_col2 * sizeof(long int));
    // represents the output matrix, which will store the result of the matrix multiplication
    long int *resultMatrix = (long int *)malloc(n_row1 * n_col2 * sizeof(long int));

    if (!matrix1 || !matrix2 || !resultMatrix) {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }


    // TODO: Parse the input csv files and fill in the input matrices
    /* ===================== Parse matrix 1 ===================== */
    
    // index tracks which matrix row is currently being read
    long int rowA = 0; 
    // read the matrix file one row at a time until all rwos read or eof reached
    while (rowA < (long int)n_row1 && fgets(line, sizeof(line), inputMatrix1) != NULL){
        // reset col-indx for cur-row
        long int colA = 0;
        // split the line into tokens using commands nad line breaks 
        char* token = strtok(line, ",\n\r");
        // parse each value int the row and store it in row-maor order
        while (token != NULL && colA < (long int)n_col1){
            // formula for row-major-order matrix, row_num * total_num_cols * cur_col
            matrix1[rowA * (long int)n_col1 + colA] = strtol(token, NULL, 10);
            colA++; // iterate row by incrementing column
            token = strtok(NULL, ",\n\r"); 
        }
        // if the number of columns read does not match expected, print error and exit
        if (colA != (long int)n_col1)
        {
            fprintf(stderr, "Matrix A row %ld has %ld cols; expected %d\n", rowA, colA, n_col1);
            free(matrix1); free(matrix2); free(resultMatrix);
            fclose(inputMatrix1); fclose(inputMatrix2); fclose(outputFile); fclose(outputTime);
            return EXIT_FAILURE;
        }

        rowA++;
    }
    // if the number of rows read does not match expected, print error and exit
    if (rowA != (long int)n_row1)
    {
        fprintf(stderr, "Matrix A file has %ld rows; expected %d\n", rowA, n_row1);
        free(matrix1); free(matrix2); free(resultMatrix);
        fclose(inputMatrix1); fclose(inputMatrix2); fclose(outputFile); fclose(outputTime);
        return EXIT_FAILURE;
    }

    /* ===================== Parse matrix 2 ===================== */
    // index tracks which matrix row is currently being read
    long int rowB = 0;
    // read the matrix file one row at a time until all rwos read or eof reached
    while (rowB < (long int)n_row2 && fgets(line, sizeof(line), inputMatrix2) != NULL){
        // reset col-indx for cur-row
        long int colB = 0;
        // split the line into tokens using commands nad line breaks
        char* token = strtok(line, ",\n\r");
        // parse each value int the row and store it in row-maor order
        while (token != NULL && colB < (long int)n_col2) {
            // formula for row-major-order matrix, row_num * total_num_cols * cur_col
            matrix2[rowB * (long int)n_col2 + colB] = strtol(token, NULL, 10);
            colB++; // iterate row by incrementing column
            token = strtok(NULL, ",\n\r");
        }
        // if the number of columns read does not match expected, print error and exit
        if (colB != (long int)n_col2)
        {
            fprintf(stderr, "Matrix B row %ld has %ld cols; expected %d\n", rowB, colB, n_col2);
            free(matrix1); free(matrix2); free(resultMatrix);
            fclose(inputMatrix1); fclose(inputMatrix2); fclose(outputFile); fclose(outputTime);
            return EXIT_FAILURE;
        }

        rowB++;
    }
    // if the number of rows read does not match expected, print error and exit
    if (rowB != (long int)n_row2)
    {
        fprintf(stderr, "Matrix B file has %ld rows; expected %d\n", rowB, n_row2);
        free(matrix1); free(matrix2); free(resultMatrix);
        fclose(inputMatrix1); fclose(inputMatrix2); fclose(outputFile); fclose(outputTime);
        return EXIT_FAILURE;
    }

    // Debug prints for testing
    if (DEBUG) {
        printf("Matrix A:\n");
        for (long int i = 0; i < (long int)n_row1; i++) {
            for (long int j = 0; j < (long int)n_col1; j++) {
                printf("%ld ", matrix1[i * (long int)n_col1 + j]);
            }
            printf("\n");
        }

        printf("Matrix B:\n");
        for (long int i = 0; i < (long int)n_row2; i++) {
            for (long int j = 0; j < (long int)n_col2; j++) {
                printf("%ld ", matrix2[i * (long int)n_col2 + j]);
            }
            printf("\n");
        }
    }

    omp_set_num_threads(thread_count);
    
    // We are interesting in timing the matrix-matrix multiplication only
    // Record the start time
    double start = omp_get_wtime();
    
    // TODO: Parallelize the matrix-matrix multiplication
    long int largest = 0;
    long int second_largest = 0;

    //create multiple threads and parallelize the outer loop of the matrix multiplication
    #pragma omp parallel
    {
        long int local_largest = 0;
        long int local_second_largest = 0;  

        #pragma omp for schedule(static)
        //iterarte through each column of matrix 2
        for (int j = 0; j < n_col2; j++){
            // iterate through each row of matrix 1
            for (int i = 0; i < n_row1; i++){
                // stores dot product of cur-row-matrix1 and cur-col-matrix2
                long int sum = 0;
                // iterate through the row of matrix 1 and column of matrix 2
                for (int k = 0; k < n_col1; k++){
                    //multiply the corresponding elements and add to sum
                    sum += matrix1[(long int)i * (long int)n_col1 + (long int)k] * matrix2[(long int)k * (long int)n_col2 + (long int)j];
                }
                //store dot product in the output matrix
                resultMatrix[(long int)i * (long int)n_col2 + (long int)j] = sum;

                //save local largest and second largest
                if (sum > local_largest) {
                    local_second_largest = local_largest;
                    local_largest = sum;
                } else if (sum > local_second_largest && sum < local_largest) { // if sum is between local lrgest and local seond largest, update local second largest
                    local_second_largest = sum;
                }
            }
        }

        #pragma omp critical
        {
            if (local_largest > largest) {
                second_largest = largest;
                largest = local_largest;
            } else if (local_largest < largest && local_largest > second_largest) { // if local largest is between global largest and global second largest, update global second largest
                second_largest = local_largest;
            }
            if (local_second_largest > second_largest && local_second_largest < largest) { // if local second largest is between global largest and global second largest, update global second largest
                second_largest = local_second_largest;
            }
        }
    }


    // Record the finish time        
    double end = omp_get_wtime();
    
    // Time calculation (in seconds)
    double time_passed = end - start;

    // Save time to file
    fprintf(outputTime, "%f", time_passed);

    // TODO: save the output matrix to the output csv file

    fprintf(outputFile, "%ld", second_largest);
    

    // Cleanup
    fclose(inputMatrix1);
    fclose(inputMatrix2);
    fclose(outputFile);
    fclose(outputTime);
   
    return 0;
}
