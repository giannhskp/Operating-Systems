#include "intListTypes.h"
#include <stdlib.h>
#include <stdio.h>    //remove it

intList createintListNode(int zipcode,int count){   //create the list node
  intList newnode=malloc(sizeof(listNode));
  newnode->zipcode=zipcode;
  newnode->count=count;
  newnode->next=NULL;
  return newnode;
}

intList insertZipCode(intList list,int zipcode){
  intList templist=list;
  if(templist==NULL){   //if list id empy  (no zipcodes have been inserted in the past)
      return createintListNode(zipcode,1);    //initialize count to 1
  }else{
    intList prev;
    while(templist!=NULL){
      if(templist->zipcode==zipcode){   //zipcode was already inserted in the past
        templist->count++;            //just increase the counter
        return list;
      }
      prev=templist;
      templist=templist->next;    //go to the next node
    }
    prev->next=createintListNode(zipcode,1);    //the zipcode was not in the list, we add it at the end
    return list;
  }
}


void deleteIntList(intList list){
  intList templist=list;
  intList next;
  for(;;) {
      if (templist->next == NULL) {   //the last node of the list
          free(templist);   //free the last node
          break;      //end
      }
      next=templist->next;    //keep the next node
      free(templist);   //free the current node
      templist=next;      //go to the next node
  }
}


void swap(intList node1,intList node2){   //get 2 nodes and swap their data
                                        //only data are swapped, not the whole node in the list
  int tempzip=node1->zipcode;
  int tempcount=node1->count;
  node1->zipcode=node2->zipcode;
  node1->count=node2->count;
  node2->zipcode=tempzip;
  node2->count=tempcount;
}


void bubbleSort(intList list){
    int swapped;      //flag that checks if any data were swapped
    intList indexptr;   //keeps the current node when traversing the list
    intList last = NULL;

    if (list == NULL)
        return;

    do
    {
        swapped = 0;    //initialize flag to 0
        indexptr = list;    //evry time set indexptr to the start of the list

        while (indexptr->next != last)
        {
            if (indexptr->count < indexptr->next->count)  //if next node's count is bigger, swap data
            {
                swap(indexptr, indexptr->next);
                swapped = 1;        //raise the flag that some data were swapped
            }
            indexptr = indexptr->next;
        }
        last = indexptr;
    }
    while (swapped);    //stop when no data were swapped
}
