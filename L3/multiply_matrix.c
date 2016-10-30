// multiply_matrix.c
/*
 * Implementierung der Matricmultiplikation.
 * Matrix[rows][columns]
 */

/* This macro is needed because I use getline, that is only available in
 * the POSIX libc.
 * So this program is not portable to Windows; it is not anyway, because
 * we have to use POSIX-Threads ;)
 */

#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "multiply_matrix.h"

/*
 * This is the parameter for multiThreadedMultiplication, because it is
 * only allowed to pass one argument to the function through#
 * pthread_create().
 */
typedef struct MatThreadArg{
    Matrix *a;
    Matrix *b;
    Matrix *c;
    int threadNumber;
    int numberOfThreads;
} MatThreadArg;

// "Private" Functions not accessible from the API for internal structure
/*
 * Encapsulate the one-threaded matrix multiplication.
 */
void oneThreadedMultiplication(Matrix *a, Matrix *b, Matrix *result);

/*
 * Encapsulate the multi-threaded matrix multiplication.
 */
void multiThreadedMultiplication(Matrix *a, Matrix *b, Matrix *result, int numThreads);

/*
 * Function executed by each worker-thread.
 * It "upacks" the MatThreadArg argument and invokes multiplyRowColumn()
 * for each threadNumber-th element.
 * If it exceeds the limit of (row*column-1) of the result matrix the
 * thread exists.
 */
void *mulitplyMatrixMT(void *argAsVoid){
    MatThreadArg *arg = (MatThreadArg*) argAsVoid;
    Matrix *a = arg->a;
    Matrix *b = arg->b;
    Matrix *resultMatrix = arg->c;


    // Iterate over each threadNumber-th element of the result-matrix; but not over it
    long elemsInResultMatrix = resultMatrix->rows * resultMatrix->columns;
    long currentElement;
    int currentRow;

    int currentColumn;
    for (currentElement = arg->threadNumber;        // Set first element to calculate
         currentElement < elemsInResultMatrix;      // Last (global) element to compute is the (row*column-1)-th
         currentElement += arg->numberOfThreads) {  // Set next element (current Index + number of Threads)

        // Calculate correct indices
        currentRow = currentElement / resultMatrix->rows;
        currentColumn = currentElement % resultMatrix->rows;
        
        // Compute value for the current element of  result matrix
        resultMatrix->matrix[currentRow][currentColumn] =
            multiplyRowColumn(a, currentRow, b, currentColumn);
    }

    pthread_exit(0);
}

/*
 * Counts the number of tokens delimited by spaces for the given line.
 * This is Used to get the number of values per line.
 */
int elemsInLine(char *line) {
    int counter = 0;
    char *currentElement = strtok(line, " ");
    while (currentElement != NULL) {
        counter++;
        currentElement = strtok(NULL, " ");
    }
    return counter - 1;
}

/*
 * Counts the number of lines in the file with the given name.
 * This is used to determine the number of rows of a source matrix.
 */
int countLines(const char filename[]) {
    int lines = 0;
    char ch;

    FILE *tmpFile = fopen(filename, "r");
    for (;;) {
        ch = getc(tmpFile);
        if (ch == '\n') {
            ++lines;
        }
        if (ch == EOF) { /* Assume the end */
            ++lines;
            break;
        }
    }
    fclose(tmpFile);

    // Return (lines - 1) because our script adds a newline at the end
    return lines - 1;
}

Matrix *readMatrix(const char filename[]) {
    assert(filename != NULL);

    int numRows = countLines(filename);
    int numColumns = 0;

    char *line = NULL;
    size_t len = 0;     // This variable is a dummy to be ignored by getline()
    ssize_t read;

    FILE *inFile = fopen(filename, "r");
    if (inFile == NULL) {
        perror("fopen()");
        return NULL;
    }

    Matrix *newMatrix = NULL;

    // Read line per line and fill the matrix
    int currentLine = 0;
    while ((read = getline(&line, &len, inFile)) != -1) {
        // Read number of columns from first line
        if(currentLine == 0) {
            char *copy = malloc(sizeof(char) * (read + 1));
            strcpy(copy, line);
            numColumns = elemsInLine(copy);
            free(copy);
            
            newMatrix = createMatrix(numRows, numColumns);
            if (newMatrix == NULL) {
                break;
            }
        }
        
        
        
        // Extract elements and write to matrix
        char *currentElement = strtok(line, " ");
        for (int i = 0; i < numColumns && currentElement != NULL; i++) {
            newMatrix->matrix[currentLine][i] = atof(currentElement);
            currentElement = strtok(NULL, " ");
        }
        
        free(line);
        line = NULL;
        currentLine++;
    }
    free(line);
    line = NULL;

    fclose(inFile);

    return newMatrix;
}

Matrix *multiplyMatrix(Matrix *a, Matrix *b, int threads) {
    // Assert that argument (matrices) are correctly
    assert(a != NULL);
    assert(b != NULL);
    assert(a->matrix != NULL);
    assert(b->matrix != NULL);
    assert(threads > 0);
    // Assert that it is possible to multiply the thẃo matrices
    assert(a->columns == b->rows);


    Matrix *result = createMatrix(a->rows, b->columns);

    // Do cool matrix multiplication
    if (threads == 1) {
        // One Thread - version
        oneThreadedMultiplication(a, b, result);
    } else if (threads > 1) {
        // concurrent computation
        multiThreadedMultiplication(a, b, result, threads);
    }
    
    return result;
}

void oneThreadedMultiplication(Matrix *a, Matrix *b, Matrix *result) {
    assert(a != NULL);
    assert(b != NULL);
    assert(result != NULL);
    assert(a->matrix != NULL);
    assert(b->matrix != NULL);
    assert(result->matrix != NULL);

    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < b->columns; j++) {
            result->matrix[i][j] = multiplyRowColumn(a, i, b, j);
        }
    }
}

void multiThreadedMultiplication(Matrix *a, Matrix *b, Matrix *result, int numThreads) {

    pthread_t pThreads[numThreads];
    MatThreadArg threadArgs[numThreads];
    int rc;

    // Create Threads
    for(int t = 0; t < numThreads; t++){
        threadArgs[t].a = a;
        threadArgs[t].b = b;
        threadArgs[t].c = result;
        threadArgs[t].threadNumber = t;
        threadArgs[t].numberOfThreads = numThreads;

    // Do Number-Crunching ;)
        rc = pthread_create(&pThreads[t], NULL, mulitplyMatrixMT,
                            (void*) &threadArgs[t]);
        assert(rc==0);
    }
    
    
    // Join Threads
    for(int t = 0; t < numThreads; t++){
        /* Wait for thread to terminate */
        rc = pthread_join(pThreads[t], NULL);
        assert(rc==0);
    }
    
    
}

double multiplyRowColumn(Matrix *a, int row, Matrix *b, int column){
    // Assert that matrices are correctly initialized
    assert(a != NULL);
    assert(b != NULL);
    assert(a->matrix != NULL);
    assert(b->matrix != NULL);
    // Assert that the row and column lie in the matrix (bounds)
    assert(row < a->rows);
    assert(column < b->columns);

    double value = 0.0;

    for (int i = 0; i < a->columns; i++) {
        value = value + a->matrix[row][i] * b->matrix[i][column];
    }

    return value;
}

Matrix *createMatrix(int numRows, int numColumns) {
    assert(numRows > 0);
    assert(numColumns > 0);

    Matrix *newMatrix = malloc(sizeof(Matrix));
    if (newMatrix == NULL) {
        perror("malloc() createMatrix");
        return NULL;
    }

    newMatrix->rows = numRows;
    newMatrix->columns = numColumns;

    newMatrix->matrix = malloc(newMatrix->rows * sizeof(double *));
    if (newMatrix->matrix == NULL) {
        perror("malloc() createMatrix");
        return NULL;
    }

    for (int i = 0; i < newMatrix->rows; i++) {
        newMatrix->matrix[i] = malloc(newMatrix->columns * sizeof(double));
        if (newMatrix->matrix[i] == NULL) {
            // Cleanup
            cleanUpMatrix(newMatrix, i - 1);
            return NULL;
        }
    }

    return newMatrix;
}

void cleanUpMatrix(Matrix *m, int lastInitializedRow) {
    assert(m != NULL);
    assert(m->matrix != NULL);
    assert(lastInitializedRow >= 0);

    for (int i = lastInitializedRow; i >= 0; i--) {
        free(m->matrix[i]);
    }
    
    free(m->matrix);
}
