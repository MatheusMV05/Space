# Nome do executável
# Define o nome do programa que será gerado após a compilação.
TARGET = space

# Diretórios
# Diretório onde estão os arquivos fonte (.c).
SRC_DIR = src
# Diretório onde estão os arquivos de cabeçalho (.h).
INCLUDE_DIR = include

# Compilador e flags
# Define o compilador a ser usado (gcc).
CC = gcc
# Adiciona o diretório de include ao caminho de pesquisa para cabeçalhos.
CFLAGS = -I$(INCLUDE_DIR)

# Bibliotecas específicas por sistema operacional
# Detecta o sistema operacional e define as bibliotecas apropriadas para linkagem.
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
    # Bibliotecas necessárias para rodar no Linux.
    LIBS = -lraylib -lGL -lpthread -ldl -lrt -lX11 -lm
else ifeq ($(UNAME), Darwin)
    # Bibliotecas necessárias para rodar no macOS (Darwin).
    LIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -lm
endif

# Arquivos fonte e objetos
# Coleta todos os arquivos .c no diretório src.
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Substitui a extensão .c por .o nos arquivos fonte para criar os objetos correspondentes.
OBJS = $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(SRCS))

# Regras
# Regra principal que será executada se nenhuma outra for especificada.
# Compila todos os objetos e cria o executável final.
all: $(TARGET)

# Regra para criar o executável a partir dos objetos.
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

# Regra para compilar arquivos .c em arquivos .o.
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para compilar e executar o programa.
# Útil para testes rápidos.
run: all
	./$(TARGET)

# Regra para limpar os arquivos compilados.
# Remove os arquivos objetos e o executável para uma recompilação limpa.
clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)

# Indica que estas regras não correspondem a arquivos reais.
.PHONY: all clean run
