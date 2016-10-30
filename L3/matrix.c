// matrix.c
/*
 * Test der Matrixmultiplikation
 * -a [filename]
 * -b [filename]
 * -t NR
 */

#include "multiply_matrix.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

void printMatrix(Matrix *matrix);


int main(int argc, char *argv[]) {

    int numThreads = 1;
    Matrix *a = NULL;
    Matrix *b = NULL;
    Matrix *result = NULL;


    // Parse input arguments
    char c;
    int p_flag = 0;
    opterr = 0;
    while( (c = getopt(argc, argv, "pa:b:t:")) != -1 )
    {
        switch(c)
        {
        case 'a':
            a = readMatrix(optarg);
            if (a == NULL) {
                printf("Fehler beim Einlesen der Datei: %s", optarg);
            }
            break;
        case 'b':
            b = readMatrix(optarg);
            if (b == NULL) {
                printf("Fehler beim Einlesen der Datei: %s", optarg);
            }
            break;
        case 't':
            numThreads = atoi(optarg);
            break;
        case 'p':
            p_flag = 1;
            break;
        case '?':
            if(optopt == 'c')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if( isprint(optopt) )	/* If option is printable */
                fprintf(stderr, "Unknown option -%c.\n", optopt);
            else
                fprintf(stderr, "Unrecognized Argument(s)!\n");
            break;
        default:
            break;
        }
    }
    
    if (p_flag == 0) {
        numThreads = 1;
    }


    // Only multiply if a and b are available
    if (a != NULL && b != NULL) {
        result = multiplyMatrix(a, b, numThreads);
        // Print the resulting matrix
        printMatrix(result);
    }


    // Clean up
    if (a != NULL) {
        cleanUpMatrix(a, a->rows - 1);
        free(a);
    }
    if (b != NULL) {
        cleanUpMatrix(b, b->rows - 1);
        free(b);
    }
    if (result != NULL) {
        cleanUpMatrix(result, result->rows - 1);
        free(result);
    }

    return 0;
}


void printMatrix(Matrix *matrix) {
    assert(matrix != NULL);
    assert(matrix->matrix != NULL);

    // Print line per line
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->columns; j++) {
            printf("%f ", matrix->matrix[i][j]);
        }
        printf("\n");
    }
}
