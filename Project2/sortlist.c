#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "sortlist.h"

/////      A very simple implementation  of a sorted list     ///////////////




list createList(){
    return NULL;
}

list insertToList(list l,node n){   // we take a node as paremeter and insert in the right place so as the list is sorted
    if(l==NULL){    // if list is empty
        list newnode=malloc(sizeof(listNode));    //create a list node
        newnode->n=malloc(sizeof(node));    // create a node to store the prime number and its info
        newnode->n->x=n.x;        // copy the prime number and its info from the given node to the one that we just created
        newnode->n->xx=n.xx;
        newnode->n->child=n.child;
        newnode->n->isTotalTime=n.isTotalTime;
        newnode->n->isCount=n.isCount;
        newnode->next=NULL;
        return newnode;     // return the new node as it is the first (and only) node of the list
    }else{
        list prev=NULL;
        list startingNode=l;
        while(l!=NULL){
            if(l->n->x>n.x){    // find the right place that the new node need to be iserted by comparing the values of the nodes
                if(prev==NULL){   // if the new node has to be inserted at the start of the list
                    list newnode=malloc(sizeof(listNode));    //same as above
                    newnode->n=malloc(sizeof(node));
                    newnode->n->x=n.x;
                    newnode->n->xx=n.xx;
                    newnode->n->child=n.child;
                    newnode->n->isTotalTime=n.isTotalTime;
                    newnode->n->isCount=n.isCount;
                    newnode->next=l;    // attach the new node to the previous first node
                    return newnode;     // return the new node as it is now the first node of the list
                }else{ // if it needs to be inserted somewhere in the middle of the list
                    list newnode=malloc(sizeof(listNode));    //same as above
                    newnode->n=malloc(sizeof(node));
                    newnode->n->x=n.x;
                    newnode->n->xx=n.xx;
                    newnode->n->child=n.child;
                    newnode->n->isTotalTime=n.isTotalTime;
                    newnode->n->isCount=n.isCount;
                    newnode->next=l;    // attach the new node with the next
                    prev->next=newnode;   // attach the new node with the previous
                    return startingNode;  // return the original starting node of the list
                }
            }else{
                prev=l;
                l=l->next;
            }
        }
        // if n is the biggest insert it at the end
        list newnode=malloc(sizeof(listNode));
        newnode->n=malloc(sizeof(node));
        newnode->n->x=n.x;
        newnode->n->xx=n.xx;
        newnode->n->child=n.child;
        newnode->n->isTotalTime=n.isTotalTime;
        newnode->n->isCount=n.isCount;
        newnode->next=NULL;
        prev->next=newnode;
        return startingNode;
    }
}

void printList(list l){
    if(l==NULL)
        printf("List is Empty\n");
    while(l!=NULL){
        printf("[ %llu , %Lf ]",l->n->x,l->n->xx);
        l=l->next;
    }
    printf("\n");
}

void deleteList(list l){
    while(l!=NULL){
        list next=l->next;
        free(l->n);
        free(l);
        l=next;
    }
}
