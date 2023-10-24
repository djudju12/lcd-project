CC=gcc

CFLAGS=-Wall -std=c99 -ggdb -pedantic -I. -I$(RAYLIB_PATH)/src

LDFLAGS=-lraylib -lm -lpthread -ldl -lrt

RAYLIB_PATH=$HOME/Software/raylib

SRC=lcd.c

OBJ=$(SRC:.c=.o)

BIN=lcd

all: $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)