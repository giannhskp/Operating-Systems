typedef struct node{
  Student student;    //pointer to student node
  struct node *next;
}Node;
typedef Node *hashNodePtr;    //pointer to the list
typedef hashNodePtr *Hash;  //double pointer to the list node (board of lists)

typedef struct hash{
  int size;   // the size of the hashTable
  int count;    //how many students are in the HashTable
  Hash table;   //the HashTable
}HashHeadNode;    // dummy node for hashTable
typedef HashHeadNode *HashTable;    //pointer to the dummy node
