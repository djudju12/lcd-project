CC=gcc

CFLAGS=-Wall -std=c99 -ggdb -pedantic -I. -I$(RAYLIB_PATH)/src

LDFLAGS=-lraylib -lm -lpthread -ldl -lrt

RAYLIB_PATH=$HOME/Software/raylib

SRC=raylib/example.c

OBJ=$(SRC:.c=.o)

BIN=example

all: $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)