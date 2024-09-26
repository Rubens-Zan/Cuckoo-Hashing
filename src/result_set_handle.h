#ifndef _RESULT_SET_H_
#define _RESULT_SET_H_

// Define DataType enum
typedef enum {
    STRING_TYPE,
    INT_TYPE
} DataType;

// Define Result_SetValue union
typedef union {
    char *s; // Pointer to a string
    int i;   // Integer value
} Result_SetValue;

// Define Result_SetElement struct
typedef struct {
    Result_SetValue value;
    DataType type;
} Result_SetElement;

// Define Result_Set struct
typedef struct {
    Result_SetElement **elements; // Array of rows, each row is an array of Result_SetElement
    DataType **types;         // Array of row types, each row has its own DataType array
    char **column_names;      // Array of column names
    int rows;
    int cols;
    int row_count; 
} Result_Set;

Result_Set* allocateResult_Set(int rows, int cols, char **column_names); 
void freeResult_Set(Result_Set *matrix); 
void printResult_Set(Result_Set *matrix, int row_count);
void printOutput(Result_Set *matrix, int row_count);

void setResult_SetValue(Result_Set *matrix, int row, int col, void *value, DataType type); 
Result_SetElement getResult_SetValue(Result_Set *matrix, int row, int col);

#endif