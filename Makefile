CC = gcc
CFLAGS = -Wall -pedantic -g -O
OBJS: mytar.o

all: mytar
mytar: mytar.o mytar.h
	$(CC) -o mytar mytar.o
mytar.o: mytar.c
	$(CC) $(CFLAGS) -c mytar.c
clean: 
	rm -f mytar.o
