// multiply_matrix.h
#ifndef MULTIPLY_MATRIX_H
#define MULTIPLY_MATRIX_H

struct Matrix {
    int rows;
    int columns;
    double **matrix;
};

typedef struct Matrix Matrix;

/*
 * Liest Matrix aus Datei "filename" aus und gibt diese als Struktur
 * (dynamisch alloziert) zurueck.
 *
 * Return: Pointer auf eingelesene Matrix
 */
Matrix *readMatrix(const char filename[]);

/*
 * Multipliziert Matrix a und b.
 *
 * Parameter threads: Anzahl der Threads die parallel die
 * Multiplikation durchfuehren sollen.
 *
 * Return: Pointer auf Ergebnismatrix
 */
Matrix *multiplyMatrix(Matrix *a, Matrix *b, int threads);

/*
 * Berechnet das Skalarprodukt der jeweiligen Zeile und Spalte der
 * uebergebenen Matrizen.
 */
double multiplyRowColumn(Matrix *a, int row, Matrix *b, int column);

/*
 * Creates a matrix with the specified number of rows and columns.
 * Returns a pointer to the newly created matrix or NULL if allocation
 * was not successfull.
 * 
 * The function allocates the memory for the struct itself, for the
 * pointers to pointers to double (rows) and for the pointers to doubles
 * (columns).
 */
Matrix *createMatrix(int numRows, int numColumns);

/*
 * Cleans up a Matrix.
 *
 * m must be a Pointer to a Matrix, that with the element matrix
 * has not all rows correctly initialized (due to malloc() failures)
 *
 * lastInitializedRow has to be the index to last successfully
 * initialized row
 * 
 * For a fully initialized matrix to be cleaned up, call:
 * cleanUpMatrix(&matrix, matrix.rows);
 * 
 * This function will free all the space pointed to by double **matrix,
 * but not the space for the struct itself. This is because it may be on
 * the stack. So you must free the pointer to the struct yourself!
 */
void cleanUpMatrix(Matrix *m, int lastInitializedRow);

#endif
