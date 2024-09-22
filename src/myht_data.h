// Estruturas de dados internas do sistema operacional

#ifndef __MYHT_DATA__
#define __MYHT_DATA__

// Estrutura que define um Task Control Block (TCB)
typedef struct result_set_t
{
  struct task_t *prev, *next ;		// ponteiros para usar em filas
  int id ;				// identificador da tarefa
} result_set_t ;

#endif