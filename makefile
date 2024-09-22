# Variáveis para os compiladores e opções de compilação
CC=gcc
CFLAGS=-O3
LDFLAGS=-lm 
DEBUG_FLAGS = -g -DDEBUG

# Nome do executável
EXECUTABLE=myht

# Caminhos
SRC_DIR=src
INCLUDE_DIR=include
OBJ_DIR=.

# Arquivos de objeto
OBJECTS=$(SRC_DIR)/myht.o $(SRC_DIR)/memory_handle.o 

# Regra padrão
all: $(EXECUTABLE)

# Regra para o executável
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	rm -f $(OBJECTS)

# Regra para arquivos objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Inclui dependências de arquivos de cabeçalho
$(SRC_DIR)/memory_handle.o: $(SRC_DIR)/memory_handle.h
# $(OBJ_DIR)/knn-mpi.o: $(INCLUDE_DIR)/knn-mpi.h $(INCLUDE_DIR)/max-heap.h
# $(OBJ_DIR)/max-heap.o: $(INCLUDE_DIR)/max-heap.h

debug : CFLAGS += $(DEBUG_FLAGS)
debug : all

# Regra para limpar os arquivos compilados
clean:
	rm  $(EXECUTABLE) $(SRC_DIR)/*.o 

purge:
	rm -f $(EXECUTABLE) 

# Evita conflito com arquivos de mesmo nome que as regras
.PHONY: all clean purge