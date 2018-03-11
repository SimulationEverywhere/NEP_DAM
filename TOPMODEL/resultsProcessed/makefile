CC=g++
CFLAGS=-std=c++17

all: ProcessResults.out 

ProcessResults.out: ProcessResults.o
	$(CC) -o ProcessResults.out ProcessResults.o

ProcessResults.o:
	$(CC) -g -c $(CFLAGS) ProcessResults.cpp -o ProcessResults.o


clean:
	rm -f *.out *.o *~
