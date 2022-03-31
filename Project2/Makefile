CFLAGS = -Wall -O3
all:
	gcc $(CFLAGS) -c sortlist.c
	gcc $(CFLAGS) -c prime1.c
	gcc $(CFLAGS) -c prime2.c
	gcc $(CFLAGS) -c prime3.c
	gcc $(CFLAGS) -c root.c
	gcc $(CFLAGS) -o myprime root.o sortlist.o
	gcc $(CFLAGS) -c middleNodes.c
	gcc $(CFLAGS) -o middleNodes middleNodes.o sortlist.o
	gcc $(CFLAGS) -c leafNodes.c
	gcc $(CFLAGS) -o leafNodes leafNodes.o prime1.o prime2.o prime3.o -lm
clean:
	rm -r sortlist.o prime1.o prime2.o prime3.o \
	 			root.o myprime middleNodes.o middleNodes \
				leafNodes.o leafNodes
