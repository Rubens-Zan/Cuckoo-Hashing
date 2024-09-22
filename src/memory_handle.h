#ifndef _MEMORY_HANDLE_H_
#define _MEMORY_HANDLE_H_

// Define DataType enum
typedef enum {
    STRING_TYPE,
    INT_TYPE
} DataType;

// Define MatrixValue union
typedef union {
    char *s; // Pointer to a string
    int i;   // Integer value
} MatrixValue;

// Define MatrixElement struct
typedef struct {
    MatrixValue value;
    DataType type;
} MatrixElement;

// Define Matrix struct
typedef struct {
    MatrixElement **elements; // Array of rows, each row is an array of MatrixElement
    DataType **types;         // Array of row types, each row has its own DataType array
    char **column_names;      // Array of column names
    int rows;
    int cols;
    int row_count; 
} Matrix;

Matrix* allocateMatrix(int rows, int cols, char **column_names); 
void freeMatrix(Matrix *matrix); 
void printMatrix(Matrix *matrix, int row_count);
void printOutput(Matrix *matrix, int row_count);

void setMatrixValue(Matrix *matrix, int row, int col, void *value, DataType type); 
MatrixElement getMatrixValue(Matrix *matrix, int row, int col);

#endif