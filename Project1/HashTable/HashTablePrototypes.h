#include "HashTableTypes.h"

HashTable htCreate(int );
hashNodePtr htInsert(HashTable ,word ,word ,word ,int ,int ,double );
void htLookup(HashTable ,word );
void htDelete(HashTable ,word );
void htDestroy(HashTable );

Student htReturnStudent(HashTable ,word );
