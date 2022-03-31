#include "../Student/StudentPrototypes.h"
#include "HashTableTypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HashTable htCreate(int size){     //Creae hash table dummy node
  HashTable head=malloc(sizeof(HashHeadNode));
  head->size=size;    //store the given size of the hash table
  head->table=malloc(head->size*(sizeof(hashNodePtr)));   //create a hash table of the given size
  for(int i=0;i<head->size;i++){
    head->table[i]=NULL;    //initialize hash table with null in every space (lists)
  }
  head->count=0;
  return head;
}

hashNodePtr createNode(Student st){   // creates a node of the student list
  hashNodePtr node=malloc(sizeof(Node));
  node->student=st;   //copy the address of the student node
  node->next=NULL;
  return node;
}


unsigned int hash(HashTable ht,word id){      //find the hash table index for the given id
  unsigned long int hashcode = 0;
  int a=33;
  word tempkey=id;
  for (; *tempkey!='\0'; tempkey++)   // for every char/number of the id
    hashcode=(a*hashcode + *tempkey) % ht->size;
  return hashcode;    //hash code is beiing %hashTableSize so it is 0<= hashcode <= ht->size
}

hashNodePtr htInsert(HashTable ht,word stId,word lName,word fName,int zipcode,int y,double g){
  int index=hash(ht,stId);
  hashNodePtr slot=ht->table[index];
  if(slot==NULL){   //no collitions so just insert the node with the student
    Student st=createStudent(stId,lName,fName,zipcode,y,g);    //create the student node
    hashNodePtr newnode=createNode(st);     //create the node of the hashTable list that contains the given student
    ht->table[index]=newnode; //insert it to the hash table, as the first (and only) node of the list in this "Bucket"
    printf("- Student %s inserted\n",getStudentId(st));
    return newnode;
  }else{    //if there are already students in this "bucket"
    hashNodePtr prev;
    while(slot!=NULL){
      if(strcmp(getStudentId(slot->student),stId)==0){    //check if the student already exists
          //printf("- Student %s exists;\n",stId);
          return NULL;
      }
      prev=slot;
      slot=slot->next;
    }
    //when we reach the last node of the list (the student does not exist)
    Student st=createStudent(stId,lName,fName,zipcode,y,g);    //create the student node
    hashNodePtr newnode=createNode(st); //create the node of the hashTable list that contains the given student
    prev->next=newnode;    // and connect it to the last node of the list
    printf("- Student %s inserted\n",getStudentId(st));
    return newnode;
  }
}

void htLookup(HashTable ht,word studentId){
  int index=hash(ht,studentId);   //find in wich bucket the student should be (using hash function)
  hashNodePtr slot=ht->table[index];    //go to this "bucket"
  if(slot==NULL){   //no student exists in this slot, so it is sure that studentId does not exist in the hash table
    printf("- Student %s does not exist\n",studentId);
  }else{
    while(slot!=NULL){    //check every student in this slot (all the collitions)
      if(strcmp(getStudentId(slot->student),studentId)==0){   //if the student is found, print his stats
          printf("- %s %s %s",getStudentId(slot->student),getLastName(slot->student),getFistName(slot->student));
          printf(" %d %d %f \n",getZip(slot->student),getYear(slot->student),getGpa(slot->student));
          return;   //we found the student so we end the function
      }
      slot=slot->next;
    }
    //if we traverse the whole list in this bucket and we dont find the student, the student does not exist in the hash table
    printf("- Student %s does not exist\n",studentId);    //the student is not in this slot
    return;
  }
}

Student htReturnStudent(HashTable ht,word studentId){   //given the student id, return the whole student node
                                                // used when deleting a student, because we need the student's year (see delete() in mngstdFunctions.c)
  int index=hash(ht,studentId);
  hashNodePtr slot=ht->table[index];
  if(slot==NULL){   //no student exists in this slot, so it is sure that studentId does not exist in the hash table
    return NULL;
  }else{
    while(slot!=NULL){    //check every student in this slot (all the collitions)
      if(strcmp(getStudentId(slot->student),studentId)==0){   //if the student is found
          return slot->student;   //we found the student so we return him
      }
      slot=slot->next;
    }
    return NULL;          //the student is not in this slot, so does not exist in the hashTable
  }
}


void deleteNode(hashNodePtr node){
  deleteStudent(node->student);   //delete the student node
  free(node);   //delete the list node
}


void htDelete(HashTable ht,word studentId){
  int index=hash(ht,studentId);
  hashNodePtr slot=ht->table[index];
  if(slot==NULL){   //no student exists in this slot, so itt is sure that studentId does not exist in the hash table
    printf("There is no student with the given id\n");
  }else{
    hashNodePtr prev=NULL;      //keep track of the previous node of the one that we are checking so as we can link him to the next one after the delete
    while(slot!=NULL){    //check every student in this slot (all the collitions)
      if(strcmp(getStudentId(slot->student),studentId)==0){   //if the student is found, delete the node
          if(prev==NULL){     //if the node to delete is the first node of the "list"
            ht->table[index]=slot->next;
            deleteNode(slot);     //free the node
          }else{
            prev->next=slot->next;
            deleteNode(slot);
          }
          printf("- Record %s deleted\n",studentId);
          return;
      }
      prev=slot;
      slot=slot->next;
    }
    printf("There is no student with the given id\n");    //the student is not in this slot
    return;
  }
}

void htDestroy(HashTable ht){   //destroys and frees the hash table (and the student nodes)
  if(ht==NULL)
    return;
  for (int i = 0; i < ht->size; ++i){   //for every bucket in the hash table
    hashNodePtr slot = ht->table[i];    //store the student list
    if (slot == NULL) {
        continue;
    }

    for(;;) {   //every node in the student list
        if (slot->next == NULL) {   //for the last node
            deleteNode(slot);     // deleteNode()  deletes the student and the list node itself
            break;
        }
        hashNodePtr next=slot->next;
        deleteNode(slot);
        slot=next;
    }
  }
  free(ht->table);    //free the board
  free(ht);   //free the dummy node of the hash table
}
