#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

// global variables that are defined in mainQuic.c file
extern int verbose;
extern int copiedEntities;
extern int bytesCopied;

///////// Linked List implementation /////////////

typedef struct l{   // list node
    char *origPath; // the path of a file in the source directory
    int origInode;  // the inode of the file in the source directory (origPath)
    char *destPath;   // the transformed path to the corresponding file in the destination
    struct l *next;   //pointer to the next node of the list
}listNode;
typedef listNode *list;

extern list hardLinkList;

void deleteDirectory(char *);	// from file delete.c

char *getDestPath(list node){
  return node->destPath;
}

list createList(){
    return NULL;
}

list insertToList(list l,int orInode,char *orPath,char *dPath,list *found){   // we take as arguments the values that the new node will contain
    if(l==NULL){    // if list is empty
        list newnode=malloc(sizeof(listNode));    //create a list node
        newnode->origInode=orInode;
        newnode->origPath=malloc(sizeof(char)*(strlen(orPath)+1));
        strcpy(newnode->origPath,orPath);
        newnode->destPath=malloc(sizeof(char)*(strlen(dPath)+1));
        strcpy(newnode->destPath,dPath);
        newnode->next=NULL;
        *found=NULL;
        return newnode;     // return the new list node as it is the first (and only) node of the list
    }else{
        list prev=NULL;
        list startingNode=l;
        // start traversing the list
        while(l!=NULL){
            if(l->origInode==orInode){  // if a file with the same inode exists
              *found=l; // then we return that node using found variable
              // and return, as we dont want to insert the new node if a file with the same inode already exists
              return startingNode;
            }
            prev=l;
            l=l->next;
        }
        // if there is no file with the same inode on the list, then we insert the new node at the end of the list
        list newnode=malloc(sizeof(listNode));  //create a new node
        newnode->origInode=orInode;
        newnode->origPath=malloc(sizeof(char)*(strlen(orPath)+1));
        strcpy(newnode->origPath,orPath);
        newnode->destPath=malloc(sizeof(char)*(strlen(dPath)+1));
        strcpy(newnode->destPath,dPath);
        newnode->next=NULL;
        prev->next=newnode; // attach it to the previous node
        *found=NULL;  // mark found as NULL, as we didnt find a file with the same inode
        return startingNode;
    }
}

void deleteList(list l){
    while(l!=NULL){
        list next=l->next;
        free(l->origPath);
        free(l->destPath);
        free(l);
        l=next;
    }
}
/////////////////////////// End of Linked list implementation /////////////////////


int checkHardLink(char *origname,char * destname){
  struct stat 	mybuf;
  struct stat 	mybufDest;
  struct stat 	firstHLcopy;
  list found;
  if(stat(origname, &mybuf)==-1){ // call stat for the source file
    return -1;
  }
  if((int)mybuf.st_nlink==1){ // if the file is not invovled in a "hardlink situation"
    // also checked before checkHardLink is called, but it was added for double checking
    return 0;
  }
  int statReturn=stat(destname,&mybufDest); //call stat for the destination file
  if ( (statReturn==-1) && (errno != ENOENT) ){ // if an error happended
    perror("stat"); return -1;
  }
  if(statReturn!=-1){ // if the destination file exists we must check if it is actually a hardlink
    if(mybuf.st_mode!=mybufDest.st_mode){ // check if the destination file is actually a hardlink
      // if not delete it
      if(S_ISDIR(mybufDest.st_mode)){	// if destination file is a directory
				deleteDirectory(destname);		// delete the directory
			}else if(S_ISLNK(mybufDest.st_mode)){	// if destination file is a sotflink
				unlink(destname);
			}else{
				remove(destname);
			}
      statReturn=-1; // mark statReturn as -1 because the destination file does not exist any more as we deleted it
    }
  }

  if( statReturn == -1){ // if the destination file does not exist
    // add the source file info and the path corresponding to the destination directory to the list
    hardLinkList=insertToList(hardLinkList,(int)mybuf.st_ino,origname,destname,&found);
    if(found==NULL){  // if no other file with the same inode already exists in the list
      // that means we should do a normal copy of the file, as this file is considered as the "original" file of a "hardlink situation"
      return 0; // so return 0 and the copy will be done by the copy file function
    }else{  // if a file with the same inode (at the source directory hierarchy) already exists in the list
      if(link(found->destPath,destname)==-1){ // then we should create a hardlink to the corresponding file in the destination directory hierarchy
        perror("Hardlink: ");
        return -1;
      }
      if(stat(destname,&mybufDest)==-1){
        perror("stat"); return -1;
      }
      bytesCopied+=(int)mybufDest.st_size;
      copiedEntities++;
      if(verbose)
        printf("Created hard link: %s\n",destname);
      return 1;
    }
  }else{  // if the destination file already exists
    // add the source file info and the path corresponding to the destination directory to the list
    hardLinkList=insertToList(hardLinkList,(int)mybuf.st_ino,origname,destname,&found);
    if(found==NULL){  // if no other file with the same inode already exists in the list
      // that means this file is considered as the "original" file of a "hardlink situation" so we should check if it is up do date as a normal file
      return 0; // so return 0 and assign that job to the copyfile function
    }else{  // if a file with the same inode (at the source directory hierarchy) already exists in the list
      // we should check if the 2 corresponding files in the destination directory refer to the same inode
      if(stat(found->destPath,&firstHLcopy)==-1){ // call stat for the "original" file of the destination directory
        perror("stat"); return -1;
      }
      if((int)firstHLcopy.st_ino != (int)mybufDest.st_ino){ // compare the inode of the "original" file with the inode of the hardlink
        // if they are different we have to update the hardlink
        // NOTE: because the "original" file is already in list, that mean that is already checked and therefore is up to date with the corresponding file in the source directory
        if(unlink(destname)==-1){ // delete the old hardlink
          perror("Unlink ");
          return -1;
        }
        if(link(found->destPath,destname)==-1){ // create a new hardlink to the "original" file of the destination file
          perror("Hardlink: ");
          return -1;
        }
        if(stat(destname,&mybufDest)==-1){
          perror("stat"); return -1;
        }
        bytesCopied+=(int)mybufDest.st_size;
        copiedEntities++;
        if(verbose)
          printf("Updated hard link: %s\n",destname);
      }else{
        if(verbose)
          printf("Up to date: %s\n",destname);
      }
      return 1;
    }
  }

}


















////
