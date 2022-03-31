// BUFFSIZE is only used when creating char boards so as to be passed as a parameter to an executable (for example see middleNodes.c lines 55-65)
#define BUFFSIZE 50
// TRANSFERBUFFSIZE is used for the size of the buffer while transfering batches from middleNodes to the root Node
// It can be changed for optimizing the transfering, considering if the range we are searching is big or small
#define TRANSFERBUFFSIZE 256
# define READ 0
# define WRITE 1

//the struct that i use to transfer every prime number and its stats.
// the same struct is used to tranfer the total time of a leaf node (when isTotalTime flag is true and time is stored in variable xx)
// also when tranfering batches from innerNodes to root, the first item of the batch is the number of elements that the batch contains
//   for this i use tha same struct with the flag isCount=1 (and count stored in variable x)

typedef struct n{
  unsigned long long int x;     //used to store the prime number (or the count if isCount flag is true)
  long double xx;       // used to store times
  int child;          // the leaf node that the prime was found mainly used when tranfering total time (to know for which child the time is)
  int isTotalTime;    // flag that shows if the total time of a child is stored in this item
  int isCount;        // flag that show if the count of a batch is stored in this item
                    // if both isTotalTime and isCount are false that means that i prime and its time is stored in this item
}node;
