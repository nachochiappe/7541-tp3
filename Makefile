#Makefile - TP3

#Variables:
EXEC=tp
CC=gcc
CFLAGS= -std=c99 -g -Wall -Wconversion -Wtype-limits -pedantic -Werror
OBJECTS=cola.o hash.o heap.o lectura.o lista.o parser.o pila.o tp3.o
VALGRIND= valgrind --leak-check=full --track-origins=yes

all: $(EXEC)

tp3: tp3.c tp3.h
	$(CC) $(CFLAGS) -c tp3.c

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXEC)

valgrind: $(EXEC)
	$(VALGRIND) ./$(EXEC)

clean: 
	rm -f *.o *~
