#Name: makefile
#Author: Brandon Cao
#Date: March 2, 2020
#Description: Makefile for pfind.
#Pledge: I pledge my honor that I have abided by the Stevens Honor System.

pfind: pfind.o pfinds.o
	gcc pfind.o pfinds.o -o pfind
pfind.o: pfind.c 
	gcc -Wall -Werror -pedantic-errors -c pfind.c
pfinds.o: pfinds.c pfinds.h
	gcc -Wall -Werror -pedantic-errors -c pfinds.c
clean:
	rm -f *.o pfind pfind.exe pfinds pfinds.exe