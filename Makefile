# Nome do executável
TARGET = space

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include

# Compilador e flags
CC = gcc
CFLAGS = -I$(INCLUDE_DIR)
LIBS = -lraylib -lGL -lpthread -ldl -lrt -lX11 -lm

# Arquivos fonte e objetos
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)

# Regras
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)

.PHONY: all clean run
