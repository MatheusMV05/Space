# Nome do executável
TARGET = space

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include

# Compilador e flags
CC = gcc
CFLAGS = -I$(INCLUDE_DIR)

# Bibliotecas específicas por sistema operacional
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
    LIBS = -lraylib -lGL -lpthread -ldl -lrt -lX11 -lm
else ifeq ($(UNAME), Darwin)
    LIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -lm
endif

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
