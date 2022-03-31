#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct t{
  int hour;
  int min;
  int sec;
  int milisec;
}time;  // stores a time

typedef struct jt{
  time start;
  time end;
}job_time;   // stores a time interval [start-end]


int compareTime(time t1,time t2){ // compares 2 times. Returns 1 if t1 is bigger, -1 if t2 is bigger and 0 if they are the same
  if(t1.hour!=t2.hour){
    return (t1.hour>t2.hour) ? 1 : -1;
  }else{
    if(t1.min!=t2.min){
      return (t1.min>t2.min) ? 1 : -1;
    }else{
      if(t1.sec!=t2.sec){
        return (t1.sec>t2.sec) ? 1 : -1;
      }else{
        if(t1.milisec!=t2.milisec){
          return (t1.milisec>t2.milisec) ? 1 : -1;
        }else{
          return 0;
        }
      }
    }
  }
}


///////// Sorted List implementation /////////////

typedef struct l{   // list node
    job_time *job;    // a time interval
    char *message;    // the message that will be printed, that contains info that show to which processes the time interval is refering to
    struct l *next;   //pointer to the next node of the list
}listNode;
typedef listNode *list;



list createList(){
    return NULL;
}

list insertToList(list l,job_time jt,char msg[]){   // we take a job_time as paremeter and insert in the right place so as the list is sorted
    if(l==NULL){    // if list is empty
        list newnode=malloc(sizeof(listNode));    //create a list node
        newnode->job=malloc(sizeof(job_time));    // create a job_time to store the time interval
        newnode->job->start=jt.start;
        newnode->job->end=jt.end;
        newnode->message=malloc(sizeof(char)*(strlen(msg)+1));
        strcpy(newnode->message,msg);
        newnode->next=NULL;
        return newnode;     // return the new list node as it is the first (and only) node of the list
    }else{
        list prev=NULL;
        list startingNode=l;
        while(l!=NULL){
            if(compareTime(jt.start,l->job->start)==0 && compareTime(jt.end,l->job->end)==0 && strcmp(msg,l->message)==0){    // if tha time interval and the message are the same (never really happend, just for safety reasons so we dont have duplicates)
              return startingNode;
            }
            if(compareTime(jt.start,l->job->start)<0){    // find the right place that the new time interval need to be iserted by comparing the job_times
                if(prev==NULL){   // if the new node has to be inserted at the start of the list

                    list newnode=malloc(sizeof(listNode));    //same as above
                    newnode->job=malloc(sizeof(job_time));    // create a job_time to store the time interval
                    newnode->job->start=jt.start;
                    newnode->job->end=jt.end;
                    newnode->message=malloc(sizeof(char)*(strlen(msg)+1));
                    strcpy(newnode->message,msg);
                    newnode->next=l;    // attach the new node to the previous first node
                    return newnode;     // return the new node as it is now the first node of the list
                }else{ // if it needs to be inserted somewhere in the middle of the list

                    list newnode=malloc(sizeof(listNode));    //same as above
                    newnode->job=malloc(sizeof(job_time));    // create a job_time to store the time interval
                    newnode->job->start=jt.start;
                    newnode->job->end=jt.end;
                    newnode->message=malloc(sizeof(char)*(strlen(msg)+1));
                    strcpy(newnode->message,msg);
                    newnode->next=l;    // attach the new node with the next
                    prev->next=newnode;   // attach the new node with the previous
                    return startingNode;  // return the original starting node of the list
                }
            }else{
                prev=l;
                l=l->next;
            }
        }
        // if jt is the biggest, insert it at the end
        list newnode=malloc(sizeof(listNode));
        newnode->job=malloc(sizeof(job_time));    // create a job_time to store the time interval
        newnode->job->start=jt.start;
        newnode->job->end=jt.end;
        newnode->message=malloc(sizeof(char)*(strlen(msg)+1));
        strcpy(newnode->message,msg);
        newnode->next=NULL;
        prev->next=newnode;
        return startingNode;
    }
}

void printList(list l){
    if(l==NULL)
        printf("List is Empty\n");
    while(l!=NULL){
        printf("[ %02d:%02d:%02d:%04d - %02d:%02d:%02d:%04d ]   %s",l->job->start.hour,l->job->start.min,l->job->start.sec,l->job->start.milisec,l->job->end.hour,l->job->end.min,l->job->end.sec,l->job->end.milisec,l->message);
        l=l->next;
    }
    printf("\n");
}

void deleteList(list l){
    while(l!=NULL){
        list next=l->next;
        free(l->job);
        free(l->message);
        free(l);
        l=next;
    }
}
/////////////////////////// End of sorted list implementation /////////////////////



list trippleConcurrency(job_time j1,job_time j3[],int j3Size,list l){
  // j1 is a time interval that saladmaker1 and saladmaker2 worked concurrently
  // so we check every active time interval of saladmaker3 so as to find a common interval
  // if we find one, that mean that all 3 saladmakers worked together
  for(int i=0;i<j3Size;i++){ // for every time interval of j3 (saladmaker3)
    if(compareTime(j3[i].start,j1.end)<0 && compareTime(j3[i].start,j1.start)>0){ // case1 that the 2 intervals have common interval
      time ccend= (compareTime(j3[i].end,j1.end)<0) ? j3[i].end : j1.end; // find the point that the common interval starts
      job_time concFound; // store the start and the end of the common interval
      concFound.start=j3[i].start;
      concFound.end=ccend;
      char message[60];   // create the message that will be printing allongside with the time interval
      sprintf(message,"All 3 SALADMAKERS worked concurrently\n");
      l=insertToList(l,concFound,message);    // insert the time interval to the sorted list
    }
    if(compareTime(j3[i].start,j1.start)<0 && compareTime(j3[i].end,j1.start)>0){ // case2 that the 2 intervals have common interval
      time ccend= (compareTime(j3[i].end,j1.end)<0) ? j3[i].end : j1.end;
      job_time concFound;
      concFound.start=j1.start;
      concFound.end=ccend;
      char message[60];
      sprintf(message,"All 3 SALADMAKERS worked concurrently\n");
      l=insertToList(l,concFound,message);
    }
  }
  return l;
}

list findConcurrentTime(job_time j1, job_time j2[],int j2Size,int id1,int id2, job_time j3[],int j3Size,list l){
  // j1 is an active time interval. We will find common time intervals of j1
  for(int i=0;i<j2Size;i++){  // for every time interval in j2
    if(compareTime(j2[i].start,j1.end)<0 && compareTime(j2[i].start,j1.start)>=0){  // case1 that the 2 time intervals are concurrent
      time ccend= (compareTime(j2[i].end,j1.end)<0) ? j2[i].end : j1.end; // find the point that the start the common interval
      job_time concFound;   // store the start and the end of the common interval
      concFound.start=j2[i].start;
      concFound.end=ccend;
      char message[60];   // create the message that shows to which saladmakers the interval is refering to
      sprintf(message,"SALADMAKER[%d] and SALADMAKER2[%d] worked concurrently \n",id1,id2);
      l=insertToList(l,concFound,message);  // insert the common time interval to the list
      if(id1==2)
        continue;
      // if findConcurrentTime is called by the fist loop of concurrentTimes find the time intervals that all 3 saladmakers worked together
      // the above if statement ensures that each trippleConcurrency interval is compouted only one time
      l=trippleConcurrency(concFound,j3,j3Size,l);
    }
    if(compareTime(j2[i].start,j1.start)<0 && compareTime(j2[i].end,j1.start)>0){ // case1 that the 2 time intervals are concurrent
      // same as above
      time ccend= (compareTime(j2[i].end,j1.end)<0) ? j2[i].end : j1.end;
      job_time concFound;
      concFound.start=j1.start;
      concFound.end=ccend;
      char message[60];
      sprintf(message,"SALADMAKER[%d] and SALADMAKER2[%d] worked concurrently \n",id1,id2);
      l=insertToList(l,concFound,message);
      if(id1==2)
        continue;
      l=trippleConcurrency(concFound,j3,j3Size,l);
    }
  }
  return l;
}

void concurrentTimes(job_time j1[],int j1Size, job_time j2[],int j2Size, job_time j3[],int j3Size){
  list l=createList();  // this list will contain all the common time intervals that we will find
  // it is a sorted list, so the time intervals will be sorted, depending on the startTime
  for(int i=0;i<j1Size;i++){    // for every active time interval of SaladMaker1
    l=findConcurrentTime(j1[i],j2,j2Size,1,2,j3,j3Size,l);  // call findConcurrentTime to find if there are common time intervals of SaladMaker2
  }
  // after the first loop we have found all the common active time intervals between saladmaker1 and SaladMaker2, and all time intervals that all 3 saladmakers were active at the same time
  for(int i=0;i<j1Size;i++){  // for every active time interval of SaladMaker1
    l=findConcurrentTime(j1[i],j3,j3Size,1,3,j2,j2Size,l);  // call findConcurrentTime to find if there are common time intervals of SaladMaker3
  }
  // after the second loop we have also found the common active time intervals between saladmaker1 and SaladMaker3
  for(int i=0;i<j2Size;i++){  // for every active time interval of SaladMaker2
    l=findConcurrentTime(j2[i],j3,j3Size,2,3,j1,j1Size,l);
  }
  // after the second loop we have also found the common active time intervals between saladmaker2 and SaladMaker3
  // so we have found all the common time intervals between all saladmakers
  printList(l); // we just print the list
  deleteList(l);  // and delete it
}

void printConcurrentTimes(int saladsMade1,int saladsMade2,int saladsMade3,int smid){
  char fileName[50]="activeTimesLogFile_";
  char fileNametemp[10];
  sprintf(fileNametemp, "%d", smid);
  strcat(fileName, fileNametemp);
  strcat(fileName, ".txt");
  FILE *fp=fopen(fileName,"r"); // open the logFile that contains the active intervals of the SaladMakers
  if(fp==NULL){
    perror("Error");
    return;
  }

  job_time s1[saladsMade1];   // active time intervals of saladmaker1 will be stored here
  job_time s2[saladsMade2];   // active time intervals of saladmaker2 will be stored here
  job_time s3[saladsMade3];   // active time intervals of saladmaker3 will be stored here
  int s1count=0,s2count=0,s3count=0;

  while(!feof(fp)){   // parse the log file and store the time intevals for each salamaker
    char buffer[100];
    char smkr[15];  // the first string of the line shows to which salad makers the interval is refering to
    if(fscanf(fp,"%s %s",smkr,buffer)<0){
      continue;
    }
    job_time jt;
    sscanf (buffer,"%d:%d:%d:%d-%d:%d:%d:%d\n",&jt.start.hour,&jt.start.min,&jt.start.sec,&jt.start.milisec,&jt.end.hour,&jt.end.min,&jt.end.sec,&jt.end.milisec);
    if(strcmp(smkr,"SALADMAKER1")==0){    // store the time interval to the corresponding saladmaker
        s1[s1count++]=jt;
    }else if(strcmp(smkr,"SALADMAKER2")==0){
        s2[s2count++]=jt;
    }else if(strcmp(smkr,"SALADMAKER3")==0){
        s3[s3count++]=jt;
    }else{
      printf("Error in log files!\n");
    }
  }
  // if a saladmaker got his ingredients immediatelly after finishing a salad, we must consider the intervals of this 2 salads as one joined interval
  job_time s1new[saladsMade1];   // active time intervals of saladmaker1, with joined intervals if the case explained in line 251 happened
  job_time s2new[saladsMade2];   // active time intervals of saladmaker2, with joined intervals if the case explained in line 251 happened
  job_time s3new[saladsMade3];   // active time intervals of saladmaker3, with joined intervals if the case explained in line 251 happened
  int s1countnew=0,s2countnew=0,s3countnew=0;
  if(s1count==1){
    s1new[s1countnew++]=s1[0];
  }else{
    for(int i=0;i<s1count;){  //for every time interval
      job_time temp;
      temp.start=s1[i].start;
      temp.end=s1[i].end;
      i++;
      for(int j=i;j<s1count;j++){   // check if the next one started immediatelly after the previous
        if(compareTime(s1[j-1].end,s1[j].start)==0){  // if so, join the two intervals and continue checking if the next one also started imediatelly after
          temp.end=s1[j].end;
          i++;
        }else{
          break;
        }
      }
      s1new[s1countnew++]=temp;   // add the new joined interval, or just the normal interval (if the second interval didn't start immediatelly after)
    }
  }
  if(s2count==1){
    s2new[s2countnew++]=s2[0];
  }else{
    for(int i=0;i<s2count;){
      job_time temp;
      temp.start=s2[i].start;
      temp.end=s2[i].end;
      i++;
      for(int j=i;j<s2count;j++){
        if(compareTime(s2[j-1].end,s2[j].start)==0){
          temp.end=s2[j].end;
          i++;
        }else{
          break;
        }
      }
      s2new[s2countnew++]=temp;
    }
  }
  if(s3count==1){
    s3new[s3countnew++]=s3[0];
  }else{
    for(int i=0;i<s3count;){
      job_time temp;
      temp.start=s3[i].start;
      temp.end=s3[i].end;
      i++;
      for(int j=i;j<s3count;j++){
        if(compareTime(s3[j-1].end,s3[j].start)==0){
          temp.end=s3[j].end;
          i++;
        }else{
          break;
        }
      }
      s3new[s3countnew++]=temp;
    }
  }

  printf("\nTime intervals: (in increasing order)\n");
  // after parsing the log file call concurrentTimes that will find the common time intervals
  concurrentTimes(s1new,s1countnew,s2new,s2countnew,s3new,s3countnew);

  fclose(fp);
}
