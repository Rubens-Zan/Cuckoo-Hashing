#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "result_set_handle.h"

Result_Set* allocateResult_Set(int rows, int cols, char **column_names) {
    Result_Set *matrix = (Result_Set*)malloc(sizeof(Result_Set));
    if (matrix == NULL) {
        printf("Memory allocation failed for matrix structure.\n");
        return NULL;
    }

    matrix->rows = rows;
    matrix->cols = cols;

    matrix->elements = (Result_SetElement**)malloc(rows * sizeof(Result_SetElement*));
    matrix->types = (DataType**)malloc(rows * sizeof(DataType*));
    matrix->column_names = (char**)malloc(rows * sizeof(char*));

    if (matrix->elements == NULL || matrix->types == NULL || matrix->column_names == NULL) {
        printf("Memory allocation failed for row pointers or column names.\n");
        free(matrix->elements);
        free(matrix->types);
        free(matrix->column_names);
        free(matrix);
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        matrix->elements[i] = (Result_SetElement*)malloc(cols * sizeof(Result_SetElement));
        matrix->types[i] = (DataType*)malloc(cols * sizeof(DataType));

        if (matrix->elements[i] == NULL || matrix->types[i] == NULL) {
            printf("Memory allocation failed for row %d.\n", i);
            for (int j = 0; j < i; j++) {
                free(matrix->elements[j]);
                free(matrix->types[j]);
            }
            free(matrix->elements);
            free(matrix->types);
            free(matrix->column_names);
            free(matrix);
            return NULL;
        }
    }

    // Copy column names
    for (int i = 0; i < rows; i++) {
        matrix->column_names[i] = strdup(column_names[i]);
        if (matrix->column_names[i] == NULL) {
            printf("Memory allocation failed for column name %d.\n", i);
            for (int j = 0; j < i; j++) {
                free(matrix->column_names[j]);
            }
            free(matrix->elements);
            free(matrix->types);
            free(matrix->column_names);
            free(matrix);
            return NULL;
        }
    }

    return matrix;
}

void setResult_SetValue(Result_Set *matrix, int row, int col, void *value, DataType type) {
    if (row > matrix->rows || col > matrix->cols) {
        printf("Index out of bounds. (%d,%d) [%d,%d]\n", row, col, matrix->rows, matrix->cols);
        return;
    }
    matrix->row_count++; 

    // Free old string if exists
    if (matrix->elements[row][col].type == STRING_TYPE && matrix->elements[row][col].value.s != NULL) {
        free(matrix->elements[row][col].value.s);
    }

    if (type == STRING_TYPE) {
        matrix->elements[row][col].value.s = strdup((char*)value);
    } else if (type == INT_TYPE) {
        matrix->elements[row][col].value.i = *((int*)value);
    }
    matrix->elements[row][col].type = type;
    matrix->types[row][col] = type;
}

Result_SetElement getResult_SetValue(Result_Set *matrix, int row, int col) {
    if (row >= matrix->rows || col >= matrix->cols) {
        printf("Index out of bounds.\n");
        Result_SetElement empty;
        return empty; // Return an empty Result_SetElement
    }
    return matrix->elements[row][col];
}

void printResult_Set(Result_Set *matrix, int row_count) {
    // Print column names
    // printf("Column Names:\n");
    for (int j = 0; j < matrix->rows; j++) {
        if (j>0){
            printf("|");
        }
        printf("\t%s", matrix->column_names[j]);
    }
    printf("\n\t"); 

    for (int i=0;i < matrix->rows * 15;i++){
        printf("-");
    }
    printf("\n"); 

    // Print matrix elements
    for (int i = 0; i < matrix->row_count; i++) {
        for (int j = 0; j < matrix->rows; j++) {
            Result_SetElement elem = matrix->elements[j][i];
            if (elem.type == STRING_TYPE) {
                printf("\t%s", elem.value.s);
            } else if (elem.type == INT_TYPE) {
                printf("\t%d ", elem.value.i);
            }
        }
        printf("\n");
    }
}

void freeResult_Set(Result_Set *matrix) {
    for (int i = 0; i < matrix->row_count; i++) {
        for (int j = 0; j < matrix->rows; j++) {
            if (matrix->elements[i][j].type == STRING_TYPE && matrix->elements[i][j].value.s != NULL) {
                free(matrix->elements[i][j].value.s);
            }
        }
        free(matrix->elements[i]);
        free(matrix->types[i]);
    }
    free(matrix->elements);
    free(matrix->types);

    for (int i = 0; i < matrix->cols; i++) {
        free(matrix->column_names[i]);
    }
    free(matrix->column_names);
    free(matrix);
}

void printOutput(Result_Set *matrix, int row_count) {
    for (int i = 0; i < matrix->row_count; i++) {
        for (int j = 0; j < matrix->rows - 1 ; j++) {
            
            Result_SetElement elem = matrix->elements[j][i];
            if (elem.type == INT_TYPE) {
                if (j == 1){
                    printf("T%d,", elem.value.i );
                } else {
                    printf("%d,", elem.value.i );
                }
            }
        }
        Result_SetElement elem = matrix->elements[matrix->rows-1][i];

        printf("%d", elem.value.i);


        printf("\n");
    }
}

/*
int main() {
    int rows = 3, cols = 3;

    // Column names
    char *column_names[] = {"Name", "Age", "Zipcode"};

    // Allocate matrix
    Result_Set *matrix = allocateResult_Set(rows, cols, column_names);
    if (matrix == NULL) {
        return 1;
    }

    // Set values in the matrix
    char *names[] = {"João", "Paulo"};
    int ages[] = {10, 20};
    int zipcodes[] = {12, 120};
    int zipcode1 = 777;
    // Row 0
    setResult_SetValue(matrix, 0, 0, names[0], STRING_TYPE);
    setResult_SetValue(matrix, 0, 1, &ages[0], INT_TYPE);
    setResult_SetValue(matrix, 0, 2, &zipcodes[0], INT_TYPE);

    // Row 1
    setResult_SetValue(matrix, 1, 0, names[1], STRING_TYPE);
    setResult_SetValue(matrix, 1, 1, &ages[1], INT_TYPE);
    setResult_SetValue(matrix, 1, 2, &zipcodes[1], INT_TYPE);
    
    // Row 2
    setResult_SetValue(matrix, 2, 2, &zipcode1, INT_TYPE);

    // Print matrix
    printf("Result_Set content:\n");
    printResult_Set(matrix);

    // Free matrix
    freeResult_Set(matrix);

    return 0;
}
*/
