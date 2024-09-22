#include <stdio.h>
#include <stdlib.h>
#include "memory_handle.h"

#include <math.h>

#define HASH_TABLE_SIZE 11
#define HASH_TABLES_N 2

#define h1(k) (k % HASH_TABLE_SIZE)
#define h2(k) ((int)floor(HASH_TABLE_SIZE * ( k * 0.9 - floor(k*0.9)) ) )

typedef enum t_state { NULL_T ,DELETED_T,VALID_T } t_state;
typedef enum t_commands { INSERT_C='i' ,DELETE_C='r',SEARCH_C='b' } t_commands;
typedef enum t_order_type { ASC_T ,DESC_T } t_order_type;
typedef enum t_datatype {INTEGER, VARCHAR } t_datatype;
typedef enum t_order_attr { VALUE_ATTR, TABLE_INDEX_ATTR, TABLE_VALUE_INDEX_ATTR  } t_order_attr;


// t_order_attr CUR_ORDERING_ATTR; 


typedef struct value_type {
    int value;
    t_state state;
} value_type;

typedef struct result_set_tuple
{
    // int value; 
    // int table_idx; // table index from value 
    // int table_value_idx; // value index on table_idx 
    void **columns_values; 
    void * col_datatype;    
    int num_columns;        // Number of columns    
} result_set_tuple;


typedef struct result_set
{
    result_set_tuple result_set_col_value[HASH_TABLE_SIZE * HASH_TABLES_N];

    Matrix *col_matrix;
    Matrix *values_matrix;

} result_set;


int compare_values(const void *a, const void *b) {
    printf("a %d e b: %d \n ", *(int*)a,*(int*) b);
    MatrixElement elem1 = * (MatrixElement *) a; 
    MatrixElement elem2 = * (MatrixElement *) b; 
    
    return (elem1.value.i - elem2.value.i);
}

/**
 * @brief Function to loop through schedule, print the escalations and print if is view equivalent and if is serializable
 * @param schedule {tSchedule *} - Schedule received
 */
void delete_value(int value, value_type* hash_table1,value_type* hash_table2){
    int hash_in_h2 = h2(value);
    int hash_in_h1 = h1(value);

    if (hash_table2[hash_in_h2].value == value){
        hash_table2[hash_in_h2].state = NULL_T;
        hash_table2[hash_in_h2].value = 0; 
    }

    if (hash_table1[hash_in_h1].value == value ){
        hash_table1[hash_in_h1].state = DELETED_T;
        hash_table1[hash_in_h1].value = 0;

    }
}; 

void insert_value(int value, value_type* hash_table1,value_type* hash_table2){
    int hash_in_table1 = h1(value);
    t_state old_val_state = hash_table1[hash_in_table1].state;
    int old_value = hash_table1[hash_in_table1].value; 
    
    // Ignora chaves duplicadas
    if (!(old_val_state == VALID_T && value == old_value )){
        hash_table1[hash_in_table1].value = value; 
        hash_table1[hash_in_table1].state = VALID_T; 

        if ( old_val_state == VALID_T ){
            int hash_old_value_t2 = h2(old_value); 
            hash_table2[hash_old_value_t2].value = old_value; 
            hash_table2[hash_old_value_t2].state = VALID_T; 
        }
    }
}

// TODO
void search_value(int value, value_type* hash_table1,value_type* hash_table2){

}

Matrix *initialize_result_set(value_type* hash_table1, value_type* hash_table2){
    char *column_names[] = {"Values", "Table Index", "Value Index"};
    int rows = HASH_TABLE_SIZE * HASH_TABLES_N, cols = 3;

    // Allocate matrix
    Matrix *res_set_matrix = allocateMatrix(3, rows, column_names);
    
    int table_indexes[] = {1, 2};
    int row_n = 0; 

    // Initialize result Set 
    for (int i=0;i< HASH_TABLE_SIZE;i++){
        int table_idx = 0 ; 
        int table_value = 0 ; 
        int table_value_idx = 1; 

        if (hash_table1[i].state == VALID_T ){
            setMatrixValue(res_set_matrix, 0, row_n,  &hash_table1[i].value, INT_TYPE);
            setMatrixValue(res_set_matrix, 1, row_n, &table_indexes[0], INT_TYPE);
            setMatrixValue(res_set_matrix, 2, row_n,  &i, INT_TYPE);            
            ++row_n;
        }

        if (hash_table2[i].state == VALID_T ){
            setMatrixValue(res_set_matrix, 0, row_n, &hash_table2[i].value, INT_TYPE);
            setMatrixValue(res_set_matrix, 1, row_n, &table_indexes[1], INT_TYPE);
            setMatrixValue(res_set_matrix, 2, row_n, &i, INT_TYPE);              
            ++row_n; 
        }
    }

    res_set_matrix->row_count = row_n; 
    return res_set_matrix; 
}

int compare_func(const void *a, const void *b) {
    MatrixElement *elem_a = (MatrixElement *) a;
    MatrixElement *elem_b = (MatrixElement *) b;

    return elem_a->value.i - elem_b->value.i;
}

void order(Matrix *input_result_set,int *order_idx,
    int order_attr_count, 
    t_order_type ordering_type, 
    Matrix *output_result_set){
    int cur_order_idx = order_idx[0];
    
    #ifdef DEBUG
    printf("\n Ordenando pelo Index: %d \n", cur_order_idx); 
    #endif

    qsort(output_result_set->elements[cur_order_idx], output_result_set->row_count, sizeof(MatrixElement), compare_func);
    
    for (int i=0;i < output_result_set->row_count ;++i){
        MatrixElement cur_elem_i  = output_result_set->elements[cur_order_idx][i];

        for (int j=0;j < input_result_set->row_count;++j){
            MatrixElement cur_elem_j  = input_result_set->elements[cur_order_idx][j];

            if (cur_elem_i.value.i == cur_elem_j.value.i){
                #ifdef DEBUG
                printf("Found equality at : %d %d \n",i,j ); 
                #endif
                for (int k=0;k < output_result_set->rows;++k){
                    if (k != cur_order_idx)
                        output_result_set->elements[k][i] = input_result_set->elements[k][j];
                }

            }
        }
    }

    #ifdef DEBUG
    printMatrix(output_result_set, output_result_set->row_count);
    #endif

    // From index 1 of the order on, I can't apply qsort without considering the before column ordering
    for (int i=1;i < order_attr_count;++i){
        cur_order_idx = order_idx[i]; 
        #ifdef DEBUG
        printf("\n Ordenando pelo Index: %d \n", cur_order_idx); 
        #endif

        for (int j = 0; j < output_result_set->row_count - 1; j++) {
            MatrixElement elem = output_result_set->elements[cur_order_idx][j];
            MatrixElement next_elem = output_result_set->elements[cur_order_idx][j+1];
            int will_switch_positions = 1;  
            if ( elem.value.i > next_elem.value.i && ordering_type == ASC_T ){
                // Change positions if equal elements regarding last ordering indexes
                for (int k=0;k < i;k++){
                    if (output_result_set->elements[order_idx[k]][j].value.i != 
                        output_result_set->elements[order_idx[k]][j+1].value.i){
                            #ifdef DEBUG
                            printf("Previous ordering already ordered, can't use order by this attribute\n");
                            #endif
                            will_switch_positions = 0;
                            break;
                    }
                }
                if (will_switch_positions == 1){
                    #ifdef DEBUG
                    printf("**** Will order ****");
                    #endif
                }

            } else if (elem.value.i > next_elem.value.i && ordering_type == DESC_T ){
                // Change positions if equal elements regarding last ordering indexes
                for (int k=0;k < i;k++){
                    if (output_result_set->elements[order_idx[k]][j].value.i != 
                        output_result_set->elements[order_idx[k]][j+1].value.i){
                            printf("Previous ordering already ordered, can't use order by this attribute \n");
                            break;
                    }
                }
                if (will_switch_positions == 1){
                    #ifdef DEBUG
                    printf("**** Will order ****");
                    #endif
                }

            }            
        }
    }
}

int main(int argc, char **argv)
{
    value_type hash_table1[HASH_TABLE_SIZE];
    value_type hash_table2[HASH_TABLE_SIZE]; 

    for (int i=0;i< HASH_TABLE_SIZE;i++){
        hash_table1[i].value = hash_table2[i].value = 0;
        hash_table1[i].state = hash_table2[i].state =  NULL_T;
    }

    char command;
    int number;
    // Continuously ead from stdin until EOF (End of File) is encountered
    while (scanf(" %c %d", &command, &number) != EOF) {
        // Process the input
        if (command == INSERT_C) {
            #ifdef DEBUG
            printf("Insert %d hashes (%d, %d)\n", number, h1(number),h2(number));
            #endif
            insert_value(number, hash_table1, hash_table2); 
        } else if (command == DELETE_C) {
            #ifdef DEBUG
            printf("Remove %d hashes (%d, %d)\n", number, h1(number),h2(number));
            #endif
            delete_value(number, hash_table1, hash_table2); 
        } else if (command == SEARCH_C){
            #ifdef DEBUG
            printf("Search value: %c\n", command);
            #endif
        }else {
            printf("Unknown command: %c\n", command);
        }
    }
    // debug
    #ifdef DEBUG
    printf("\n*******************************\n"); 
    for (int i=0;i < HASH_TABLE_SIZE; i++){
        printf("Table 1: Index: %d Value: %d \n",i,hash_table1[i].value); 
    }

    printf("\n"); 
    for (int i=0;i < HASH_TABLE_SIZE; i++){
        printf("Table 2: Index: %d Value: %d \n",i,hash_table2[i].value); 
    }
    printf("*******************************\n"); 
    #endif
    // TODO
    
    result_set final_result_set; 
    
    // Fazer a parte de ordenação da saída
    Matrix *res_set_matrix = initialize_result_set(hash_table1, hash_table2 ); 
    Matrix *output_set_matrix_final = initialize_result_set(hash_table1, hash_table2 ); 

    char *column_names[] = {"Values", "Table Index", "Value Index"};
    int rows = HASH_TABLE_SIZE * HASH_TABLES_N, cols = 3;

    // Allocate matrix
    #ifdef DEBUG
    printMatrix(res_set_matrix, res_set_matrix->row_count);
    #endif
    int order_idx[3] = {0,1,2}; 
    int order_attr_count = 3; 
    order(res_set_matrix, order_idx, order_attr_count, ASC_T, output_set_matrix_final ); 
    #ifdef DEBUG
    printMatrix(output_set_matrix_final, output_set_matrix_final->row_count);
    #endif
    t_order_attr order_attr[3]; 
    printOutput(output_set_matrix_final, output_set_matrix_final->row_count);
    
    
    free(output_set_matrix_final);
    free(res_set_matrix);

    
    return 0;
};