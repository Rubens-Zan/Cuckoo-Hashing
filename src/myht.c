#include <stdio.h>
#include <stdlib.h>
#include "result_set_handle.h"

#include <math.h>

#define HASH_TABLE_SIZE 11
#define HASH_TABLES_N 2

#define h1(k) (k % HASH_TABLE_SIZE)
#define h2(k) ((int)floor(HASH_TABLE_SIZE * ( k * 0.9 - floor(k*0.9)) ) )

// Enum com tipos de estados de um registro na tabela
typedef enum t_state { NULL_T ,DELETED_T,VALID_T } t_state;
// enum com tipos de comandos aceitos
typedef enum t_commands { INSERT_C='i' ,DELETE_C='r',SEARCH_C='b' } t_commands;
// enum com formato da ordenação, utilizada ordenação ascendente 
typedef enum t_order_type { ASC_T ,DESC_T } t_order_type;
// enum com tipos de datatypes aceitos neste projeto
typedef enum t_datatype {INTEGER, VARCHAR } t_datatype;
// enum com indices aceitos para odenação, esta sendo utilizado o indice de value_attr 
typedef enum t_order_attr { VALUE_ATTR, TABLE_INDEX_ATTR, TABLE_VALUE_INDEX_ATTR  } t_order_attr;

typedef struct value_type {
    int value;
    t_state state;
} value_type;


int compare_values(const void *a, const void *b) {
    printf("a %d e b: %d \n ", *(int*)a,*(int*) b);
    Result_SetElement elem1 = * (Result_SetElement *) a; 
    Result_SetElement elem2 = * (Result_SetElement *) b; 
    
    return (elem1.value.i - elem2.value.i);
}

/// @brief Função para deleção do valor de entrada dentre as tabelas 
/// @param value Valor a ser excluido
/// @param hash_table1 1a tabela de armazenamento 
/// @param hash_table2 2a tabela de armazenamento
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

/// @brief Função que realiza inserção de valores nas tabelas hash determinadas
/// @param value Valor a ser inserido em uma das tabelas
/// @param hash_table1 1a tabela de armazenamento 
/// @param hash_table2 2a tabela de armazenamento
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

/// @brief Função para inicialização do result_set, fazendo varredura nas tabelas de hash. 
/// @param hash_table1 1a tabela hash
/// @param hash_table2 2a tabela hash
/// @return Result_Set com os registros válidos unidos das duas tabelas utilizadas
Result_Set *initialize_result_set(value_type* hash_table1, value_type* hash_table2){
    char *column_names[] = {"Values", "Table Index", "Value Index"};
    int rows = HASH_TABLE_SIZE * HASH_TABLES_N, cols = 3;

    // Allocate matrix
    Result_Set *res_set_matrix = allocateResult_Set(3, rows, column_names);
    
    int table_indexes[] = {1, 2};
    int row_n = 0; 

    // Initialize result Set 
    for (int i=0;i< HASH_TABLE_SIZE;i++){
        int table_idx = 0 ; 
        int table_value = 0 ; 
        int table_value_idx = 1; 

        if (hash_table1[i].state == VALID_T ){
            setResult_SetValue(res_set_matrix, 0, row_n,  &hash_table1[i].value, INT_TYPE);
            setResult_SetValue(res_set_matrix, 1, row_n, &table_indexes[0], INT_TYPE);
            setResult_SetValue(res_set_matrix, 2, row_n,  &i, INT_TYPE);            
            ++row_n;
        }

        if (hash_table2[i].state == VALID_T ){
            setResult_SetValue(res_set_matrix, 0, row_n, &hash_table2[i].value, INT_TYPE);
            setResult_SetValue(res_set_matrix, 1, row_n, &table_indexes[1], INT_TYPE);
            setResult_SetValue(res_set_matrix, 2, row_n, &i, INT_TYPE);              
            ++row_n; 
        }
    }

    res_set_matrix->row_count = row_n; 
    return res_set_matrix; 
}

/// @brief Função de comparação a ser utilizada no qsort
/// @param a Elemento a a ser comparado
/// @param b Elemento b a ser comparado
/// @return Resultado ascendente
int compare_func(const void *a, const void *b) {
    Result_SetElement *elem_a = (Result_SetElement *) a;
    Result_SetElement *elem_b = (Result_SetElement *) b;

    return elem_a->value.i - elem_b->value.i;
}

/// @brief Função de ordenação de um result_set,
/// @param input_result_set Result_set de entrada 
/// @param order_idx Array de indexes do result_set a ser utilizado para a ordenação 
/// @param order_attr_count Quantidade de order_idx 
/// @param ordering_type Tipo da ordenação, utilizando formato Ascendente
/// @param output_result_set Result_Set a ser utilizado como saida após a ordenação 
void order(Result_Set *input_result_set,int *order_idx,
    int order_attr_count, 
    t_order_type ordering_type, 
    Result_Set *output_result_set){
    int cur_order_idx = order_idx[0];
    
    #ifdef DEBUG
    printf("\n Ordenando pelo Index: %d \n", cur_order_idx); 
    #endif

    qsort(output_result_set->elements[cur_order_idx], output_result_set->row_count, sizeof(Result_SetElement), compare_func);
    
    for (int i=0;i < output_result_set->row_count ;++i){
        Result_SetElement cur_elem_i  = output_result_set->elements[cur_order_idx][i];

        for (int j=0;j < input_result_set->row_count;++j){
            Result_SetElement cur_elem_j  = input_result_set->elements[cur_order_idx][j];

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
    printResult_Set(output_result_set, output_result_set->row_count);
    #endif

    // From index 1 of the order on, I can't apply qsort without considering the before column ordering
    for (int i=1;i < order_attr_count;++i){
        cur_order_idx = order_idx[i]; 
        #ifdef DEBUG
        printf("\n Ordenando pelo Index: %d \n", cur_order_idx); 
        #endif

        for (int j = 0; j < output_result_set->row_count - 1; j++) {
            Result_SetElement elem = output_result_set->elements[cur_order_idx][j];
            Result_SetElement next_elem = output_result_set->elements[cur_order_idx][j+1];
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
                            #ifdef DEBUG
                            printf("Previous ordering already ordered, can't use order by this attribute \n");
                            #endif
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
    
    // Fazer a parte de ordenação da saída
    Result_Set *res_set_matrix = initialize_result_set(hash_table1, hash_table2 ); 
    Result_Set *output_set_matrix_final = initialize_result_set(hash_table1, hash_table2 ); 

    char *column_names[] = {"Values", "Table Index", "Value Index"};
    int rows = HASH_TABLE_SIZE * HASH_TABLES_N, cols = 3;

    // Allocate matrix
    #ifdef DEBUG
    printResult_Set(res_set_matrix, res_set_matrix->row_count);
    #endif
    int order_idx[3] = {0,1,2}; 
    int order_attr_count = 3; 
    order(res_set_matrix, order_idx, order_attr_count, ASC_T, output_set_matrix_final ); 
    #ifdef DEBUG
    printResult_Set(output_set_matrix_final, output_set_matrix_final->row_count);
    #endif
    t_order_attr order_attr[3]; 
    printOutput(output_set_matrix_final, output_set_matrix_final->row_count);
    
    free(output_set_matrix_final);
    free(res_set_matrix);
    
    return 0;
};