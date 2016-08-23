
This repository contains solutions to some of the programming exercises found in
the textbook _Parallel Programming in C with MPI and OpenMP_ by Michael Quinn.

********************

The following programs are included in the repository.  

1. **Motivations and History**

2. **Parallel Architectures**

3. **Parallel Algorithm Design**

4. **Message-Passing Progamming**

	* `exer04_06.c`: hello, world program
	
    * `exer04_07.c`: compute the sum `1 + 2 + ... + p`
	
    * `exer04_08.c`: find the number of times that two consecutive odd numbers
	  are both prime in the set of integers from `2` to `n`
	  
	* `exer04_09.c`: find the largest gap between prime numbers in the set of
      integers from `2` to `n`
	  
	* `exer04_10.c`: number of 6-digit ID combinations subject to some
      restrictions
	  
	* `exer04_11.c`: calculate an integral using the rectangle method
	
	* `exer04_12.c`: calculate an integral using Simpson's rule
	
5. **The Sieve of Eratosthenes**

    * `sieve_quinn`: Quinn's version of the Sieve of Eratosthenes
	
	* `exer05_06.c`: Modify Sieve algorithm so as to not set aside memory for
      even numbers
	  
    * `exer05_07.c`: Modify Sieve algorithm so that each process finds prime
      numbers between `2` and `floor( sqrt(n) )`, rather than waiting for a
      broadcast from the `0`-th process
	  
    * `exer05_08.c`: Modify Sieve algorithm to improve the cache hit rate by
      decomposing the section of numbers each process is responsible for into
      further sub-blocks
	  
    * `exer05_09.c`: Functional decomposition of Sieve algorithm

    * `exer05_11.c`: Compute `1/1 + 1/2 + ... + 1/n` for some choice of `n`
	
********************
