#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Use more libraries as necessary

#define DEBUG 0     // for submission
// #define DEBUG 1  // for testing


/* ---------- Project 1 - Problem 2 - Mat-Vec Mult ----------
    This file will multiply a matrix and vector.
    Complete the TODOs left in this file.
*/ // ------------------------------------------------------ //


int main (int argc, char *argv[])
{
    // Catch console errors
    if( argc != 7)
    {
        printf("USE LIKE THIS: serial_mult_mat_vec in_mat.ok csv n_row_1 n_col_1 in_vec.csv n_row_2 output_file.csv \n");
        return EXIT_FAILURE;
    }

    // stack-allocated character bugger an array of characters holds one ine of text read from a file
    char line[10240];  


    // Get the input files
    FILE *matFile = fopen(argv[1], "r");    // get the matrix file -> "matrix.csv" 1th arg
    FILE *vecFile = fopen(argv[4], "r");    // get the vector file -> "vector.csv" 4th arg

    // Get dim of the matrix
    // char* p1;    // dont need these since we changed to long
    // char* p2;    // dont need these since we changed to long
    long int n_row1 = strtol(argv[2], NULL, 10);        // number of rows in input matrix
    long int n_col1 = strtol(argv[3], NULL, 10);        // number of cols in input matrix

    // Get dim of the vector
    // char* p3;    // dont need these since we changed to long
    long int n_row2 = strtol(argv[5], NULL, 10);        // size of vector number of rows since vector has 1 col

    // Get the output file
    FILE *outputFile = fopen(argv[6], "w");


    // TODO: Use malloc to allocate memory for the matrices
    // represent the entire input matrix stores as a 1-dimensional array in row-major order
    long int *matrix = (long int *)malloc(n_row1 * n_col1 * sizeof(long int));      // malloc here requests a block of memory of specified size from the heap at runtime and returns a pointer to the start of that memory
    // represents the entire vector
    long int *vector = (long int *)malloc(n_row2 * sizeof(long int));
    // represents the output vector output of the multiplication
    long int *result = (long int *)malloc(n_row1 * sizeof(long int));

    if (!matrix || !vector || !result) {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }

    


    // TODO: Parse the input CSV files
    // ========== Parse vector ONCE ==========
    long int i = 0;
    // read the vector file line at a time until expected rows are read or eof is there
    while (i < n_row2 && fgets(line, sizeof(line), vecFile) != NULL) {
        // pointer used by strtol to mark where it stops
        char *endptr;
        // convert the cur-line to a long-int, store it in the vector at ith pos
        vector[i] = strtol(line, &endptr, 10);
        i++;
    }
    if (i != n_row2) {
        fprintf(stderr, "Vector file has fewer than %ld rows\n", n_row2);
        // cleanup...
        return EXIT_FAILURE;
    }

    // ========== Parse matrix ONCE ==========
    // index tracks which matrix row is currently being read
    long int row = 0;
    // read the matrix file one row at a time until all rwos read or eof reached
    while (row < n_row1 && fgets(line, sizeof(line), matFile) != NULL) {
        // reste col-indx for cur-row
        long int col = 0;
        // split the line into tokens using commands nad line breaks 
        char *token = strtok(line, ",\n\r");

        // parse each value int the row and store it in row-maor order
        while (token != NULL && col < n_col1) {
            // formula for row-major-order matrix, row_num * total_num_cols * cur_col
            matrix[row * n_col1 + col] = strtol(token, NULL, 10);
            col++;  // iterte row by increment col
            token = strtok(NULL, ",\n\r");
        }

        if (col != n_col1) {
            fprintf(stderr, "Matrix row %ld has %ld cols; expected %ld\n", row, col, n_col1);
            // cleanup...
            return EXIT_FAILURE;
        }

        row++;
    }
    if (row != n_row1) {
        fprintf(stderr, "Matrix file has %ld rows; expected %ld\n", row, n_row1);
        // cleanup...
        return EXIT_FAILURE;
    }


    // Debug vector paring
    if (DEBUG) {
        printf("Vector:\n");
        for (long int i = 0; i < n_row2; i++) {
            printf("%ld\n", vector[i]);
        }
    }
    // Debug matrix parsing
    if (DEBUG) {
        printf("Matrix:\n");
        for (long int i = 0; i < n_row1; i++) {
            for (long int j = 0; j < n_col1; j++) {
                printf("%ld ", matrix[i * n_col1 + j]);
            }
            printf("\n");
        }
    }


    if (row != n_row2) {
        fprintf(stderr, "Vector file has fewer than %ld rows\n", n_row2);
        free(matrix); free(vector); free(result);
        fclose(matFile); fclose(vecFile); fclose(outputFile);
        return EXIT_FAILURE;
    }


    // TODO: Perform the matrix-vector multiplication

    // iterate each row in matrix
    for (long int r = 0; r < n_row1; r++) {
        // stores dot product of cur-row-matrix
        long int sum = 0;   
        // iterate over each col for cur-row
        for (long int c = 0; c < n_col1; c++) {
            // multiply mat[r][c] elemnent of matrix at row-r, col-c by vector elemtn c-indx
            sum += matrix[r * n_col1 + c] * vector[c];
        }
        // store dot product for row-r in output vector
        result[r] = sum;
    }


    // TODO: Write the output CSV file
    for (long int r = 0; r < n_row1; r++) {
        fprintf(outputFile, "%ld\n", result[r]);
    }


    // TODO: Free memory
    free(matrix);
    free(vector);
    free(result);


    // Cleanup
    fclose (matFile);
    fclose (vecFile);
    fclose (outputFile);


    // Free buffers here as well!
    // TBD


    return 0;
}


// testing run command: ./serial_mult_mat_vec matrix.csv 2 2 vector.csv 2 out.csv
/*
make clean
make

SAMPLE TEST:
./serial_mult_mat_vec matrix.csv 2 2 vector.csv 2 out.csv

*/