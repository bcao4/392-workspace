CC = gcc
CFLAGS = -g -Wall -Werror -pedantic-errors

all: main
main: main.o libadd.a
	$(CC) $(CFLAGS) main.c -o main libadd.a
libadd.a: add.c add.h
	$(CC) $(CFLAGS) -c add.c
	ar rs libadd.a add.o
clean:
	rm -f *.o *.a main main.exe