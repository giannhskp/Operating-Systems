#include "../Student/StudentPrototypes.h"
#include "../HashTable/HashTableTypes.h"
#include "InvertedIndexTypes.h"
#include "./intList/intListPrototypes.h"
#include <time.h>

int currentYear(){  //find the current year using system's time
  time_t now;
  time(&now);
  struct tm *local = localtime(&now);
  int year = local->tm_year + 1900;
  return year;
}


InvertedIndex iiCreate(){   //create an empty InvertedIndex
  InvertedIndex ii=malloc(sizeof(InvIndNode));
  ii->year=-1;      //initialized value, usedonly when InvertedIndex's first node is created
  ii->list=NULL;
  ii->next=NULL;
  ii->count=0;
  return ii;
}

InvIndList createListNode(hashNodePtr hashNode){    //create an InvertedIndex node
  InvIndList node=malloc(sizeof(Node));
  node->hashTableNode=hashNode;     //keep tha address of the hashTable node that contains the student
  node->next=NULL;
  return node;
}


InvIndList insertToList(InvIndList list,hashNodePtr hashNode){    //sorted by gpa (bigger first)
  InvIndList templist=list;
  InvIndList prev;
  if(list==NULL){
    return createListNode(hashNode);
  }
  if(getGpa(templist->hashTableNode->student)<getGpa(hashNode->student)){   //if new node need to be inserted at the first place
    InvIndList newnode=createListNode(hashNode);
    newnode->next=templist;
    return newnode;
  }else{  //else skip first node
    prev=templist;
    templist=templist->next;
  }
  while(templist!=NULL){
    if(getGpa(templist->hashTableNode->student)<getGpa(hashNode->student)){
      InvIndList newnode=createListNode(hashNode);    //create the node
      newnode->next=templist;   //connecet it witth the other nodes
      prev->next=newnode;
      return list;      //return the first node of the list
    }
    prev=templist;
    templist=templist->next;
  }
  //if the new student has the lower gpa is inserted at the end
  prev->next=createListNode(hashNode);
  return list;
}




InvertedIndex iiInsert(InvertedIndex ii,hashNodePtr hashNode){
  InvertedIndex tempii=ii;
  InvertedIndex prev=NULL;
  int studyYears=currentYear()-getYear(hashNode->student)+1;     //compute the studing year of the student using the current year
  if(tempii->year == -1){   // if inverted index is empty
      tempii->year=studyYears;
      tempii->list=createListNode(hashNode);
      tempii->count++;
      return ii;
  }
  while(tempii!=NULL){
      if(tempii->year == studyYears){  //some students with the same year already exist in the list
        tempii->list=insertToList(tempii->list,hashNode); //so we just insert the hashTable node that contains the student at the list of this year
        tempii->count++;
        return ii;
      }
      if(tempii->year>studyYears){     //there are not other students with the same year
          InvertedIndex newYear=iiCreate();   // so we need to create a new node for this year
          newYear->year=studyYears;
          newYear->list=createListNode(hashNode);   // and then insert the hashTable node that contains the student at the (empty) list of that year
          newYear->count++;
          newYear->next=tempii;
          if(prev==NULL){    //the year that we are inserting is the smaller one on the list
                            //so we need to insert the new node at the start of the list
            return newYear;
          }else{        //insert the new node somewhere in the middle of the list
            prev->next=newYear;
            return ii;
          }
      }
      prev=tempii;
      tempii=tempii->next;
  }
  //if the list ended and we didn't find the year, we insert a new node with this year at the end of the InvIndList
  // because is the biggest of all year
  InvertedIndex newYear=iiCreate();
  newYear->year=studyYears;
  newYear->list=createListNode(hashNode);
  newYear->count++;
  newYear->next=NULL;
  prev->next=newYear;
  return ii;
}

int iiNumber(InvertedIndex ii,int year){
  InvertedIndex tempii=ii;
  if(tempii->year == -1){   // if inverted index is empty
      return 0;
  }
  while(tempii!=NULL){
      if(tempii->year == year){  //some students of this year  exist in the list
        return tempii->count;
      }
      if(tempii->year>year){     //there are no students of this year
          return 0;
      }
      tempii=tempii->next;
  }
  return 0;
}

void iiTop(InvertedIndex ii,int num,int year){
  InvertedIndex tempii=ii;
  if(tempii->year == -1){   // if inverted index is empty
      printf("- No students enrolled in %d\n",year);
      return;
  }
  while(tempii!=NULL){
      if(tempii->year == year){  //some students of this year  exist in the list
          if(tempii->count == 0){   // if inverted index is empty
              printf("- No students enrolled in %d\n",year);
              return;
          }
          int count=1;
          InvIndList studList=tempii->list;
          if(studList==NULL){   //no students in this year
            printf("- No students enrolled in %d\n",year);
            return;
          }
          //students are ordered in the list by getGpa, so print the num first students
          while(studList!=NULL && count<=num){
            printf("%s ",getStudentId(studList->hashTableNode->student));
            studList=studList->next;    //go to the next student
            count++;    //count how many students we have printed
            if(studList!=NULL && count<=num)
              printf(", ");
          }
          printf("\n");
          if(count<=num)    //if there are less than num students
            printf("There are only %d students in year %d. Printed all of them.\n",count-1,year);
          return;
      }
      if(tempii->year>year){     //there are no students of this year
          printf("- No students enrolled in %d\n",year);
          return ;
      }
      tempii=tempii->next;
  }
  return;
}

void iiMinimum(InvertedIndex ii,int year){
  InvertedIndex tempii=ii;
  if(tempii->year == -1){   // if inverted index is empty
      printf("- No students enrolled in %d\n",year);
      return;
  }
  while(tempii!=NULL){
      //student list for every year is sorted by gpa (bigger->smaller)
      //so we just need the last student of the list of this certain year, and those who have the same gpa as the last one
      if(tempii->year == year){  //some students of this year  exist in the list
        if(tempii->count == 0){   // if inverted index is empty
            printf("- No students enrolled in %d\n",year);
            return;
        }
        InvIndList studList=tempii->list;
        InvIndList prev;
        if(studList==NULL){   //no students in this year
          printf("- No students enrolled in %d\n",year);
          return;
        }
        while(studList!=NULL){  //go to the last node
            prev=studList;
            studList=studList->next;
        }
        Student last=prev->hashTableNode->student;   //the last node of the student list
        double minGpa=getGpa(last);
        printf("- %s",getStudentId(last));
        studList=tempii->list;
        while(studList!=NULL){  //get all students with the same gpa as the last one
            //we can't compare if doubles are equal, so we check if they are almost equal
            if((getGpa(studList->hashTableNode->student)-minGpa<0.00001) && strcmp(getStudentId(studList->hashTableNode->student),getStudentId(last))!=0){
              printf(", %s",getStudentId(studList->hashTableNode->student));
            }
            studList=studList->next;
        }
        printf("\n");
        return;
      }
      if(tempii->year>year){     //there are no students of this year
          printf("- No students enrolled in %d\n",year);
          return;
      }
      tempii=tempii->next;
  }
  printf("- No students enrolled in %d\n",year);
  return;
}

double iiAverage(InvertedIndex ii,int year){
  InvertedIndex tempii=ii;
  if(tempii->year == -1){   // if inverted index is empty
      return -1.0;
  }
  while(tempii!=NULL){    //search all the years to find the one we want
      if(tempii->year == year){  //some students of this year  exist in the list
        if(tempii->count==0)
            return -1.0;
        InvIndList studList=tempii->list;
        double sumGpa=0.0;    //for every student of this year sum up their gpa
        if(studList==NULL){   //no students in this year
          return -1.0;
        }
        while(studList!=NULL){
            sumGpa+=getGpa(studList->hashTableNode->student);  //sum up all  the gpas
            studList=studList->next;
        }
        return sumGpa/(tempii->count);
        // tempii->count contains how many students there are in this year
      }
      if(tempii->year>year){     //there are no students of this year
          return -1.0;
      }
      tempii=tempii->next;
  }
  return -1.0;
}

void iiCount(InvertedIndex ii){
  InvertedIndex tempii=ii;
  if(tempii->year == -1){   // if inverted index is empty
      printf("- No students are enrolled\n");
      return;
  }
  int printed=0;
  while(tempii!=NULL){    //traverse all the nodes to print stats for every year
      //we already keep count of every year, so for every node we print the year and the count
      if(tempii->count==0){
          tempii=tempii->next;
          if(tempii==NULL)
              printf("\n");
          else
              printf(" , ");
          continue;
      }
      printf("{%d,%d}",tempii->year,tempii->count);
      printed=1;
      tempii=tempii->next;
      if(tempii!=NULL){
        if(tempii->count!=0)
            printf(" , ");
      }else{
        printf("\n");
      }
  }
  if(!printed)
    printf("- No students are enrolled\n");
  return;
}


void iiPostalCode(InvertedIndex ii,int n){
  InvertedIndex tempii=ii;
  if(tempii->year == -1){   // if inverted index is empty
      printf("- No students are enrolled\n");
      return;
  }
  intList ziplist=NULL;   //list that every node contains a zipcode and a counter for this zipcode
  //for every student of every year
  while(tempii!=NULL){  //for every year
      InvIndList studList=tempii->list; //get student list
      while(studList!=NULL){  //for every student in this year
          ziplist=insertZipCode(ziplist,getZip(studList->hashTableNode->student));   //insert the zip code to the list (insertZipCode() in intListImplementtation.c)
          studList=studList->next;
      }
      tempii=tempii->next;
  }
  if(ziplist==NULL){
    printf("- No students are enrolled\n");
    return;
  }
  bubbleSort(ziplist); //sort the ziplist (bubbleSort() implemented in intListImplementtation.c)
  int count=1;  //keep the current rank in the list
  int many=0;
  intList FistNodeOfZipList=ziplist;
  printf("- ");
  while(ziplist!=NULL){
    if(count==n){   //if the current rank is the rank we want
      printf("%d",ziplist->zipcode);  //print the zipcode
      if(ziplist->next!=NULL){
        if(ziplist->next->count ==  ziplist->count){  //if the next zipcode has the same count
          printf(", ");
          many=1; //flag that there are many zipcodes in this rank (used for printing purposes bellow)
          //dont increase the count so the next zipcode is also printed
        }else{  //if the next node has not the same count
          count++;    //increase the counter  and stop
          break;
        }
      }
    }else{  //if we are not in thye rank we want
      if(ziplist->next!=NULL){
        if(ziplist->next->count !=  ziplist->count){    //if the next node has not the same count
          count++;  //increase the count
        }
        //if the next zipcode (node) has the same count the counter is not increased because they are in the same rank
      }
    }
    ziplist=ziplist->next;
  }
  deleteIntList(FistNodeOfZipList);   //free the list
  if(count<n){
    printf("There is no %d most popular. Either there are not so many zipcodes or they are tied in higher ranks.\n",n);
    return;
  }
  if(many)
    printf(" are %d most popular\n",n);
  else
    printf(" is %d most popular\n",n);
}




void deleteListNode(InvIndList node){
  //student will be deleted from the hashTable
  free(node);
}




InvIndList deleteFromList(InvIndList list,Student st){  //delete the given student from the student list
  InvIndList templist=list;
  InvIndList prev;
  if(list==NULL)
    return NULL;
  if(strcmp(getStudentId(templist->hashTableNode->student),getStudentId(st))==0){    //if the student we want to delete is the first on the list
    InvIndList next=templist->next;   //keep tha second student
    deleteListNode(templist);   //delete him
    return next;      //return the second student
  }else{      //else skip first student
    prev=templist;
    templist=templist->next;
  }
  while(templist!=NULL){
    if(strcmp(getStudentId(templist->hashTableNode->student),getStudentId(st))==0){
      prev->next=templist->next;
      deleteListNode(templist);
      return list;
    }
    prev=templist;
    templist=templist->next;
  }
  //when we reach the last node of the list
  return list;
}



void iiDelete(InvertedIndex ii,Student st){   //delete the given student from the InvertedIndex
  // the student is not actually deleted. Only the node that contains the pointer to the hashTable node (and the student)
  // the student is deleted from htDelete() (HashTable) that is called always after this function
  InvertedIndex tempii=ii;
  int studyYears=currentYear()-getYear(st)+1;
  if(tempii->year == -1){   // if inverted index is empty
      return;
  }
  while(tempii!=NULL){
      if(tempii->year == studyYears){  //we found the year we want
        tempii->list=deleteFromList(tempii->list,st); //so we just delete the student using deleteFromList function
        tempii->count--;    //decrease the student counter
        return;
      }
      if(tempii->year>studyYears){     //there is no student in the given year
          return;
      }
      tempii=tempii->next;
  }
}



void deleteList(InvIndList list){ //free a year list
  InvIndList next;
  for(;;) {
      if(list==NULL)
        return;
      if (list->next == NULL) {   //the last node
          deleteListNode(list);     //delete the node and stop
          break;
      }
      next=list->next;    //keep the next node
      deleteListNode(list);   //delete the current node
      list=next;    //go to the next one
  }

}



void iiDestroy(InvertedIndex ii){     //delete (free) the InvertedIndex
  //student are not deleted (freed)
  //student are deleted when we delete the HashTable
  //this function is only called before the HashTable destroyer
  InvertedIndex next;
  for(;;) {     // for every node (year) of the inverted index
      if (ii->next == NULL) {   //the last node (year)
          deleteList(ii->list);   //delete the student list (using the above function that frees the list)
          free(ii);     //free the node
          break;
      }
      next=ii->next;    //keep the next node
      deleteList(ii->list); //delete the student list of the current node(using the above function that frees the list)
      free(ii);   //free the current node
      ii=next;  //go to the next one
  }
}
