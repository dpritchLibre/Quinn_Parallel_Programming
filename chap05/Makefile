
CC = mpicc
CFLAGS = -Wall -g3

executables = sieve_quinn exer05_06 exer05_07 exer05_08 exer05_11


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

exer05_11: exer05_11.o parse_args.o
	$(CC) $(CFLAGS) exer05_11.o parse_args.o -o exer05_11


# object file construction ---------------------------------

exer05_06.o : exer05_06.c sieve_helper.h parse_args.h
	$(CC) $(CFLAGS) -c exer05_06.c

exer05_07.o : exer05_07.c sieve_helper.h parse_args.h
	$(CC) $(CFLAGS) -c exer05_07.c

exer05_08.o : exer05_08.c sieve_helper.h parse_args.h
	$(CC) $(CFLAGS) -c exer05_08.c

exer05_11.o : exer05_11.c parse_args.h
	$(CC) $(CFLAGS) -c exer05_11.c

sieve_helper.o : sieve_helper.c mpi_helper.h
	$(CC) $(CFLAGS) -c sieve_helper.c

parse_args.o : parse_args.c
	$(CC) $(CFLAGS) -c parse_args.c


# file cleanup ---------------------------------------------

.PHONY : clean clean_o clean_execut

clean :
	rm *.o $(executables) -f

clean_o :
	rm *.o -f

clean_execut :
	rm $(executables) -f

