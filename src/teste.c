#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define DataType enum
typedef enum {
    INT,
    STRING
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
    MatrixElement **elements; // Array of rows, each row is an array of MatrixElement (transposed: row of same "column")
    DataType **types;         // Array of row types, each row has its own DataType array
    char **column_names;      // Array of column names
    int rows;
    int cols;
} Matrix;

// Comparison function for qsort with multiple rows acting as columns
int compare_columns(const void *a, const void *b, void *order_data) {
    Matrix *matrix = ((Matrix **)order_data)[0];  // Matrix data
    int *columns = ((int **)order_data)[1];       // Array of column indices (sorted priorities)
    int num_columns = ((int *)order_data)[2];     // Number of columns to sort by
    
    int col1 = *(const int *)a;
    int col2 = *(const int *)b;

    // Compare based on the sorted order of columns (rows in this case)
    for (int i = 0; i < num_columns; i++) {
        int row = columns[i];  // The row we're treating as a "column"

        MatrixElement elem1 = matrix->elements[row][col1];
        MatrixElement elem2 = matrix->elements[row][col2];

        // Compare based on the data type of the current row
        if (elem1.type == INT && elem2.type == INT) {
            if (elem1.value.i != elem2.value.i) {
                return elem1.value.i - elem2.value.i;  // Compare integers
            }
        } else if (elem1.type == STRING && elem2.type == STRING) {
            int result = strcmp(elem1.value.s, elem2.value.s);
            if (result != 0) {
                return result;  // Compare strings
            }
        } else {
            // Define a custom order if the types differ
            return (elem1.type == INT) ? -1 : 1;
        }
    }

    return 0;  // Treat as equal if all compared rows/columns are equal
}

// Function to sort matrix columns based on transposed rows with priority
void sort_matrix_by_columns(Matrix *matrix, int *columns_to_sort_by, int num_columns) {
    // Create an array of column indices
    int *column_indices = malloc(matrix->cols * sizeof(int));
    for (int i = 0; i < matrix->cols; i++) {
        column_indices[i] = i;
    }

    // Prepare sorting data
    void *order_data[3] = { matrix, columns_to_sort_by, &num_columns };

    // Use qsort_r to sort columns based on the row data (acting as "columns")
    qsort_r(column_indices, matrix->cols, sizeof(int), compare_columns, order_data);

    // Reorder the matrix columns based on the sorted indices
    MatrixElement **sorted_elements = malloc(matrix->rows * sizeof(MatrixElement *));
    for (int row = 0; row < matrix->rows; row++) {
        sorted_elements[row] = malloc(matrix->cols * sizeof(MatrixElement));
        for (int col = 0; col < matrix->cols; col++) {
            sorted_elements[row][col] = matrix->elements[row][column_indices[col]];
        }
    }

    // Replace the original elements with the sorted ones
    for (int row = 0; row < matrix->rows; row++) {
        free(matrix->elements[row]);
        matrix->elements[row] = sorted_elements[row];
    }

    free(sorted_elements);
    free(column_indices);
}

int main() {
    // Create an example matrix
    Matrix matrix;
    matrix.rows = 3;
    matrix.cols = 3;

    // Allocate memory for the matrix elements
    matrix.elements = malloc(matrix.rows * sizeof(MatrixElement *));
    for (int i = 0; i < matrix.rows; i++) {
        matrix.elements[i] = malloc(matrix.cols * sizeof(MatrixElement));
    }

    // Fill the matrix with example values (each row is actually a "column")
    // Row 0 (Column 0): "banana", "apple", "cherry"
    matrix.elements[0][0].value.s = "banana";
    matrix.elements[0][0].type = STRING;

    matrix.elements[0][1].value.s = "apple";
    matrix.elements[0][1].type = STRING;

    matrix.elements[0][2].value.s = "cherry";
    matrix.elements[0][2].type = STRING;

    // Row 1 (Column 1): 10, 20, 30
    matrix.elements[1][0].value.i = 10;
    matrix.elements[1][0].type = INT;
    matrix.elements[1][1].value.i = 20;
    matrix.elements[1][1].type = INT;
    matrix.elements[1][2].value.i = 30;
    matrix.elements[1][2].type = INT;

    // Row 2 (Column 2): 5, 3, 8
    matrix.elements[2][0].value.i = 5;
    matrix.elements[2][0].type = INT;
    matrix.elements[2][1].value.i = 3;
    matrix.elements[2][1].type = INT;
    matrix.elements[2][2].value.i = 8;
    matrix.elements[2][2].type = INT;

    // Specify the rows (columns in a database-like context) to sort by, in priority order
    int columns_to_sort_by[] = { 0 };  // Sort by fruits, then the third row (quantities), then second row (price)
    for (int row = 0; row < matrix.rows; row++) {
        for (int col = 0; col < matrix.cols; col++) {
            if (matrix.elements[row][col].type == INT) {
                printf("%d ", matrix.elements[row][col].value.i);
            } else if (matrix.elements[row][col].type == STRING) {
                printf("%s ", matrix.elements[row][col].value.s);
            }
        }
        printf("\n");
    }
    // Sort the matrix by multiple columns
    sort_matrix_by_columns(&matrix, columns_to_sort_by, 3);

    // Print the sorted matrix
    printf("Sorted Matrix:\n");
    for (int row = 0; row < matrix.rows; row++) {
        for (int col = 0; col < matrix.cols; col++) {
            if (matrix.elements[row][col].type == INT) {
                printf("%d ", matrix.elements[row][col].value.i);
            } else if (matrix.elements[row][col].type == STRING) {
                printf("%s ", matrix.elements[row][col].value.s);
            }
        }
        printf("\n");
    }

    // Free the allocated memory
    for (int i = 0; i < matrix.rows; i++) {
        free(matrix.elements[i]);
    }
    free(matrix.elements);

    return 0;
}
