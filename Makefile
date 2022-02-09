# -*- Makefile -*-

CC = mpicc #compiler 

CFLAGS = -Wall -g  #compiler flags

OBJFLAGS =  -c -Wall -I ./includes/ 

BINARIES = main 

all : main.o utils.o bb.o est_d.o
	$(CC) $(CFLAGS) -o main main.c src/utils.c src/branch_and_bound.c src/estructuras_de_datos.c

main.o: main.c
	$(CC) -c -Wall main.c
	
est_d.o: src/estructuras_de_datos.c
	$(CC) $(OBJFLAGS) src/estructuras_de_datos.c

bb.o: src/branch_and_bound.c
	$(CC) $(OBJFLAGS) src/branch_and_bound.c

utils.o: src/utils.c
	$(CC) $(OBJFLAGS) src/utils.c

.PHONY: clean
clean:
	-rm -f *.o $(BINARIES) 
