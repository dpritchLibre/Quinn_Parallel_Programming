
CC = mpicc
CFLAGS = -Wall -g3

executables = exer04_06 exer04_07 exer04_08 exer04_09 exer04_10 exer04_11 \
	exer04_12


all : $(executables)


# executable construction ----------------------------------

exer04_06 : exer04_06.c
	$(CC) $(CFLAGS) exer04_06.c -o exer04_06

exer04_07 : exer04_07.c
	$(CC) $(CFLAGS) exer04_07.c -o exer04_07

exer04_08 : exer04_08.o prime_num_fcns.o parse_n.o
	$(CC) $(CFLAGS) exer04_08.o prime_num_fcns.o parse_n.o \
	-lm -o exer04_08

exer04_09 : exer04_09.o prime_num_fcns.o parse_n.o
	$(CC) $(CFLAGS) exer04_09.o prime_num_fcns.o parse_n.o \
	-lm -o exer04_09

exer04_10 : exer04_10.c
	$(CC) $(CFLAGS) exer04_10.c -o exer04_10

exer04_11 : exer04_11.o parse_n.o
	$(CC) $(CFLAGS) exer04_11.o parse_n.o -o exer04_11

exer04_12 : exer04_12.o parse_n.o
	$(CC) $(CFLAGS) exer04_12.o parse_n.o -o exer04_12


# object file construction ---------------------------------

exer04_08.o : exer04_08.c prime_num_fcns.h
	$(CC) $(CFLAGS) -c exer04_08.c

exer04_09.o : exer04_09.c prime_num_fcns.h
	$(CC) $(CFLAGS) -c exer04_09.c

exer04_11.o : exer04_11.c
	$(CC) $(CFLAGS) -c exer04_11.c

exer04_12.o : exer04_12.c prime_num_fcns.h
	$(CC) $(CFLAGS) -c exer04_12.c

prime_num_fcns.o : prime_num_fcns.c
	$(CC) $(CFLAGS) -c prime_num_fcns.c -lm

parse_n.o : parse_n.c
	$(CC) $(CFLAGS) -c parse_n.c


# file cleanup ---------------------------------------------

.PHONY : clean clean_o clean_execut

clean :
	rm *.o $(executables) -f

clean_o :
	rm *.o -f

clean_execut :
	rm $(executables) -f
