gcc src/main.c src/screen.c src/timer.c src/keyboard.c -Iinclude -lraylib -lGL -lpthread -ldl -lrt -lX11 -lm

# gcc src/main.c src/screen.c src/timer.c src/keyboard.c \
#   -Iinclude \
#   -I/opt/homebrew/include \
#   -L/opt/homebrew/lib \
#   -lraylib -lm
#   Só funciona assim no Mac, no Linux não precisa disso