all:
	gcc -c mainQuic.c
	gcc -c copy.c
	gcc -c delete.c
	gcc -c hardlinks.c
	gcc -o quic mainQuic.o copy.o delete.o hardlinks.o

clean:
	rm -rf *.o quic
