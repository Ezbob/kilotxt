CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic

TARGET=kilotxt
SRC=$(wildcard *.c)
HDR=$(wildcard *.h)

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $^ -o $(TARGET)

