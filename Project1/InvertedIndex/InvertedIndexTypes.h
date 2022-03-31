typedef struct inode{
  hashNodePtr hashTableNode;    //pointer to the hash table node that contains he student
  struct inode *next;
}iNode;

typedef iNode *InvIndList;  // pointer to an InvertedIndex list node

typedef struct iinode{
  int year;     //the year that all the students in students list are in
  int count;    //keeps counter of how many student there are in the students list
  InvIndList list;      //lists of hashTable nodes that contain students of this year
  struct iinode *next;
}InvIndNode;      //the node of the inverted index
typedef InvIndNode *InvertedIndex;      //pointer to the node
