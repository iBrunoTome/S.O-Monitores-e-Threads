all:METODOS
	gcc -pthread main.c -omain -Wall -pedantic -ansi
METODOS:
	gcc -c main.c
