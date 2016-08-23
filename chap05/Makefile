
CC = mpicc
CFLAGS = -Wall -g3

executables = sieve_quinn exer05_06 exer05_07 exer05_08


all : $(executables)


# executable construction ----------------------------------

sieve_quinn : sieve_quinn.c
	$(CC) $(CFLAGS) sieve_quinn.c -lm -o sieve_quinn

exer05_06 : exer05_06.o sieve_helper.o parse_args.o
	$(CC) $(CFLAGS) exer05_06.o sieve_helper.o parse_args.o -lm -o exer05_06

exer05_07 : exer05_07.o sieve_helper.o parse_args.o
	$(CC) $(CFLAGS) exer05_07.o sieve_helper.o parse_args.o -lm -o exer05_07

exer05_08 : exer05_08.o sieve_helper.o parse_args.o
	$(CC) $(CFLAGS) exer05_08.o sieve_helper.o parse_args.o -lm -o exer05_08


# object file construction ---------------------------------

exer05_06.o : exer05_06.c parse_args.h
	$(CC) $(CFLAGS) -c exer05_06.c

sieve_helper.o : sieve_helper.c mpi_helper.h
	$(CC) $(CFLAGS) -c sieve_helper.c

parse_args.o : parse_args.c
	$(CC) $(CFLAGS) -c parse_args.c
