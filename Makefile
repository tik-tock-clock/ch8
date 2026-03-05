CC=gcc
LIBS=-lSDL2

BUILD_DIR=build
PROJECT=ch8

FILES = src/main.c

all:

	$(CC) $(FILES) -o $(BUILD_DIR)/$(PROJECT) $(LIBS)

clean:

	rm -rf $(BUILD_DIR)/