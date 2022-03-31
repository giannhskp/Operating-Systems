#include "./Student/StudentPrototypes.h"
#include "./HashTable/HashTablePrototypes.h"
#include "./InvertedIndex/InvertedIndexPrototypes.h"


typedef struct dnode{
  HashTable ht;
  InvertedIndex ii;
}dataNode;
typedef dataNode *Data;   // keep both data (hashTable and InvertedIndex) in one "variable"
