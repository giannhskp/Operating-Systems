MODULES = .

CFLAGS = -g -Wall -O3

PROGRAM = demo

OBJS = $(MODULES)/mngstd.o

OBJS += $(MODULES)/mngstdFunctions.o
CFLAGS += -I$(MODULES)/Student
OBJS += $(MODULES)/Student/Student.o
CFLAGS += -I$(MODULES)/HashTable
OBJS += $(MODULES)/HashTable/HashTableImplementation.o
CFLAGS += -I$(MODULES)/InvertedIndex
OBJS += $(MODULES)/InvertedIndex/InvertedIndexImplementation.o
CFLAGS += -I$(MODULES)/InvertedIndex/intList
OBJS += $(MODULES)/InvertedIndex/intList/intListImplementation.o





$(PROGRAM): clean $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(PROGRAM)

clean:
	rm -f $(PROGRAM) $(OBJS)

run: $(PROGRAM)
	./$(PROGRAM)
