all:
	gcc -g -Wall  -c   chef.c
	gcc -g -Wall  -c   findConcurrentTimes.c
	gcc -o chef chef.o findConcurrentTimes.o -pthread
	gcc -g -Wall  -c   saladmaker.c
	gcc -o saladmaker saladmaker.o -pthread

clean:
	rm -rf *.o chef saladmaker
