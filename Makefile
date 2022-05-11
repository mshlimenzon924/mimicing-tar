CC = gcc
CFLAGS = -Wall -pedantic -g -O

all: mytar
mytar: mytar.o mytar.h ttable.o
	$(CC) -o mytar mytar.o
mytar.o: mytar.c
	$(CC) $(CFLAGS) -c mytar.c
ttable.o:
	$(CC) $(CFLAGS) -c ttable.c
clean: 
	rm -f mytar.o