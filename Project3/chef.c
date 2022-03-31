#include "header.h"
#include "findConcurrentTimes.h"


int main(int argc, char* argv[]){
  int id;
  smData *smLoc;
  int pid=getpid();
  int numofSlds;
  int mantime;
  time_t t;
  srand((unsigned) time(&t));

  if(argc!=5){
    printf("Wrong input\n");
    return 0;
  }else{  //read command line arguments
    int count=1;
    int inputRead=0;    // for wrong input check
    while(count<=3){
      if(strcmp("-n",argv[count])==0){
        numofSlds=atoi(argv[count+1]);  // number of salads
        inputRead++;
        count+=2;
        continue;
      }
      if(strcmp("-m",argv[count])==0){
        mantime=atoi(argv[count+1]);    // man time for resting
        inputRead++;
        count+=2;
        continue;
      }
    }
    if(inputRead<2){    // check if both arguments were read
      printf("Wrong input\n");
      return 0;
    }
  }



  //creating shared memory
  if ((id = shmget(IPC_PRIVATE, sizeof(smData), (S_IRUSR|S_IWUSR))) == -1) {
       perror("Failed to create shared memory segment");
       return -1;
   }
   printf("[CHEF] created shared memory with id %d    (must be given as parameter to saladmakers)\n",id);
   if ((smLoc = (smData *)shmat(id, NULL, 0)) == (void *)-1) {
       perror("Failed to attach memory segment");
       return 1;
   }
   //initalizing shared memory data
   for(int i=0;i<3;i++){
     smLoc->finished[i]=FALSE;    // points if a saladmaker process is ended
   }
   for(int i=0;i<3;i++){
     smLoc->saladsMade[i]=0;    // holds the number of salads each saladmaker has made. Every time a saladmaker makes a salades, he increases the value by one
   }
   for(int i=0;i<3;i++){
     smLoc->pid[i]=0;     // stores the process id of each saladmaker
   }
   smLoc->saladsToBeMade=numofSlds;     // stores the total number of salads that have to be made.
   // Every time a saladmaker makes a salad, he decreases the value by one
   //
   // create the joined logFile. The name of the file contains the smid, so every time tha name id different (so as to keep all the log files from previous execitions)
   char fileName[FILENAME_SIZE]="joinedLogFile_";
   char fileNametemp[10];
   sprintf(fileNametemp, "%d", id);
   strcat(fileName, fileNametemp);
   strcat(fileName, ".txt");
   FILE *joinedLogFile=fopen (fileName, "a");   // open the file. (usually it does not exist so it is created)
   if(joinedLogFile==NULL){   // check for error while opening the file
     perror("Error");
     return -1;
   }
   /////
   sem_unlink(MUTEX_SEMAPHORE_NAME);  // checks if the semaphore exists, if it exists we unlink him from the process
   sem_t *mutex=sem_open(MUTEX_SEMAPHORE_NAME, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 1); // create the semaphore
   // mutex semaphore is use to acheive mutual exclusion while processes access (and read or modify) the shared memory
   if (mutex!= SEM_FAILED)    //check for error while opening the semaphore
      printf("[CHEF] successfully created new semaphore!\n");
   else if (errno== EEXIST ) {    // semaphore already exists
	    printf("semaphore appears to exist already!\n");
	    mutex = sem_open(MUTEX_SEMAPHORE_NAME, 0);
	 }
   else{  // an other error occured
     assert(mutex != SEM_FAILED);
   }
   //////
   sem_unlink(TABLE_FULL_SEMAPHORE_NAME1);  // same as above
   sem_t *tableFullSemaphore1=sem_open(TABLE_FULL_SEMAPHORE_NAME1, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 0);
   // tableFullSemaphore1 is used to notify saladmaker1 when his ingredients are selected (and placed to the table) by the chef
   // when the chef has not yet selected saladmaker1's ingredients, SaladMaker1 is suspended on this semaphore (see saladmaker.c)
   if (tableFullSemaphore1!= SEM_FAILED)
      printf("[CHEF] successfully created new semaphore!\n");
   else if (errno== EEXIST ) {
	    printf("semaphore appears to exist already!\n");
	    tableFullSemaphore1 = sem_open(TABLE_FULL_SEMAPHORE_NAME1, 0);
	 }
   else{
     assert(tableFullSemaphore1 != SEM_FAILED);
   }
   //////
   sem_unlink(TABLE_FULL_SEMAPHORE_NAME2);
   sem_t *tableFullSemaphore2=sem_open(TABLE_FULL_SEMAPHORE_NAME2, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 0);
   // same as tableFullSemaphore1 except is is used for notifing saladmaker2
   if (tableFullSemaphore2!= SEM_FAILED)
      printf("[CHEF] successfully created new semaphore!\n");
   else if (errno== EEXIST ) {
	    printf("semaphore appears to exist already!\n");
	    tableFullSemaphore2 = sem_open(TABLE_FULL_SEMAPHORE_NAME2, 0);
	 }
   else{
     assert(tableFullSemaphore2 != SEM_FAILED);
   }
   /////
   sem_unlink(TABLE_FULL_SEMAPHORE_NAME3);
   sem_t *tableFullSemaphore3=sem_open(TABLE_FULL_SEMAPHORE_NAME3, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 0);
   // same as tableFullSemaphore1 except is is used for notifing saladmaker3
   if (tableFullSemaphore3!= SEM_FAILED)
      printf("[CHEF] successfully created new semaphore!\n");
   else if (errno== EEXIST ) {
	    printf("semaphore appears to exist already!\n");
	    tableFullSemaphore3 = sem_open(TABLE_FULL_SEMAPHORE_NAME3, 0);
	 }
   else{
     assert(tableFullSemaphore3 != SEM_FAILED);
   }
   /////
   sem_unlink(TABLE_EMPTY_SEMAPHORE_NAME);
   sem_t *tableEmptySemaphore=sem_open(TABLE_EMPTY_SEMAPHORE_NAME, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 1);
   // tableEmptySemaphore is showing if the table is full of ingredient (= the corresponding saladmaker has not yet got his ingredients)
   // if table is not empty chef is susspended on tableEmptySemaphore until the saladmaker gets his ingredients and notifies the chef that the table is now empty (by doing sem_post(tableEmptySemaphore))
   if (tableEmptySemaphore!= SEM_FAILED)
      printf("[CHEF] successfully created new semaphore!\n");
   else if (errno== EEXIST ) {
	    printf("semaphore appears to exist already!\n");
	    tableEmptySemaphore = sem_open(TABLE_EMPTY_SEMAPHORE_NAME, 0);
	 }
   else{
     assert(tableEmptySemaphore != SEM_FAILED);
   }
   /////
   sem_unlink(END_SEMAPHORE_NAME);
   sem_t *endSemaphore=sem_open(END_SEMAPHORE_NAME, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 0);
   // endSemaphore shows if the numofSlds where made.
   // When chef gives the ingredients for the last salad, he is suspended at endSemaphore until the last salad is made
   // when the last salad is made, the saladmaker that made the last salad posts endSemaphore and notifies chef that salads are made
   if (endSemaphore!= SEM_FAILED)
      printf("[CHEF] successfully created new semaphore!\n");
   else if (errno== EEXIST ) {
	    printf("semaphore appears to exist already!\n");
	    endSemaphore = sem_open(END_SEMAPHORE_NAME, 0);
	 }
   else{
     assert(endSemaphore != SEM_FAILED);
   }
   /////
   sem_unlink(JOIN_LOG_SEMAPHORE_NAME);
   sem_t *joinLogSemaphore=sem_open(JOIN_LOG_SEMAPHORE_NAME, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 1);
   // this semaphore is used so as to ensure that only one process is writing at a time at the joined logFile
   if (joinLogSemaphore!= SEM_FAILED)
      printf("[CHEF] successfully created new semaphore!\n");
   else if (errno== EEXIST ) {
	    printf("semaphore appears to exist already!\n");
	    joinLogSemaphore = sem_open(JOIN_LOG_SEMAPHORE_NAME, 0);
	 }
   else{
     assert(joinLogSemaphore != SEM_FAILED);
   }
   /////
   int prev=-1;   // keep the previous selection of the chef so as not to select 2 consecutive times the same ingredients
   int random;    // used to select the random ingredients
   time_t T= time(NULL);  // used for getting the current time
   struct  tm currtime;   // used for getting the current time
   struct timeval  nn;    // used for getting the current time
  for(int salad=1;salad<=numofSlds;salad++){
      sem_wait(tableEmptySemaphore);

      if(salad!=1){ //if it is the forst salad skip resting
        T= time(NULL);
        currtime = *localtime(&T);    // get current time
        gettimeofday(&nn, NULL);    //get  current milliseconds
        sem_wait(joinLogSemaphore);   // ensure that no one is writing at the joinedLogFile
          // write on the joinedLogFile
          fprintf(joinedLogFile,"[%02d:%02d:%02d:%04d] [1] [Chef]  %d seconds for resting\n",currtime.tm_hour, currtime.tm_min, currtime.tm_sec,(int)(nn.tv_usec / 1000),mantime);
          fflush(joinedLogFile);  // empty the buffer so as the line will imediatelly be writen on the file
        sem_post(joinLogSemaphore);
        sleep(mantime);   // rest for mantime seconds
      }
      //selecting the 2 ingredients. We need to select 2 random ingredients out of 3.
      //So we can simplys call rand for the ingredient that we will not select and the 2 remaining are the ingredients that chef selects
      do{
        random=rand()%3;    // select a random ingredient (that is the ingredient that the chef will not select)
      }while(random==prev);   // check if it is not the same as the previous, if it is select another one
      prev=random;
      T= time(NULL);    //  get current time that will be used in the message
      currtime = *localtime(&T);
      gettimeofday(&nn, NULL);
      char buffer[130];
      // create the message depending on the selection of the chef
      sprintf(buffer,"[%02d:%02d:%02d:%04d] [%d] [Chef]  Selecting ingredients [ ",currtime.tm_hour, currtime.tm_min, currtime.tm_sec,(int)(nn.tv_usec / 1000),pid);
      for(int i=0;i<3;i++){
          if(random==i)
            continue;
          if(i==ONION)
            strcat(buffer,"ONION ");
          if(i==GREEN_PEPPER)
            strcat(buffer,"GREEN_PEPPER ");
          if(i==TOMATO)
            strcat(buffer,"TOMATO ");
      }
      strcat(buffer,"]\n");

      sem_wait(joinLogSemaphore);   // write the message on the joined log file, when no other process is writing
        fprintf(joinedLogFile,"%s",buffer);
        fflush(joinedLogFile);
      sem_post(joinLogSemaphore);

      T= time(NULL);    //get current time
      currtime = *localtime(&T);
      gettimeofday(&nn, NULL);
      sem_wait(joinLogSemaphore); // write the message on the joined log file, when no other process is writing
        fprintf(joinedLogFile,"[%02d:%02d:%02d:%04d] [%d] [Chef]  Notifing saladaker #%d \n",currtime.tm_hour, currtime.tm_min, currtime.tm_sec,(int)(nn.tv_usec / 1000),pid,random+1);
        fflush(joinedLogFile);
      sem_post(joinLogSemaphore);
      //notify the  corresponding SaladMaker depending on the ingredients that chef selected
      if(random==0){
        sem_post(tableFullSemaphore1);    // post the semaphore that saladmaker1 is suspended so as to "wake him up"
      }else if(random==1){
        sem_post(tableFullSemaphore2);  // post the semaphore that saladmaker2 is suspended so as to "wake him up"
      }else{
        sem_post(tableFullSemaphore3);  // post the semaphore that saladmaker3 is suspended so as to "wake him up"
      }
 }
 //when chef has placed all the ingredients needed so as to make numofSlds salads

 sem_wait(tableEmptySemaphore);   // wait for the SaladMaker to pick up the last 2 ingredients

 // when the saladmaker gets the ingredients, do the last break
 T= time(NULL);   // get current time
 currtime = *localtime(&T);
 gettimeofday(&nn, NULL);
 sem_wait(joinLogSemaphore);  // write the message on the joined log file, when no other process is writing
  fprintf(joinedLogFile,"[%02d:%02d:%02d:%04d] [%d] [Chef]  %d seconds for resting\n",currtime.tm_hour, currtime.tm_min, currtime.tm_sec,(int)(nn.tv_usec / 1000),pid,mantime);
  fflush(joinedLogFile);
 sem_post(joinLogSemaphore);

 sleep(mantime);    // do the last break for mantime second
 sem_wait(endSemaphore);    // wait for the last saladmaker to end the last salad
 // when the last salad is being made, the other 2 saladmaker (except the one that creates the last salad) are suspended and waiting for ingredients
 // so when the last salad is made "wake them up", in order to realise that all salads are made and then end.
 // when a SaladMaker wakes up from tableFullSemaphore he checks immediatelly if all salads are made an if that's true they finish (see saladmaker.c)
 sem_post(tableFullSemaphore1);
 sem_post(tableFullSemaphore2);
 sem_post(tableFullSemaphore3);


  int saladmakersFinished=FALSE;
  // wait for all saladmaker processes to be completed
  while(saladmakersFinished==FALSE){
   sem_wait(mutex);
   if(smLoc->finished[0]==TRUE && smLoc->finished[1]==TRUE && smLoc->finished[2]==TRUE)
      saladmakersFinished=TRUE;
   sem_post(mutex);
  }
  printf("[CHEF] All saladmakers ended\n");

  printf("\nTotal #salads created: [%d]\n",numofSlds);
  int totalSaladsMade[3]; //storing the number of salads that each saladmakers created, so as to use it in printConcurrentTimes
  for(int i=0;i<3;i++){
    sem_wait(mutex);  // all other processes have ended, but just to be sure
    printf("#salads of SaladMaker%d [%d] : %d\n",i+1,smLoc->pid[i],smLoc->saladsMade[i]); // get from shared memory how many salads each saladmaker has made
    totalSaladsMade[i]=smLoc->saladsMade[i];    // store it so as to be used afterwards
    sem_post(mutex);
  }
  // call printConcurrentTimes so as to find and print the common time intervals between the processes
  // the first 3 arguments are the number of salads that each saladmaker created, and the last one is the shared memory id so as to locate the logFile (as it contains the id in its name)
  printConcurrentTimes(totalSaladsMade[0],totalSaladsMade[1],totalSaladsMade[2],id);    // see findConcurrentTimes.c

  // close all semaphores
  sem_close(mutex);
  sem_close(tableFullSemaphore1);
  sem_close(tableFullSemaphore2);
  sem_close(tableFullSemaphore3);
  sem_close(tableEmptySemaphore);
  sem_close(joinLogSemaphore);
  sem_close(endSemaphore);
  fclose(joinedLogFile);
  if ( shmctl (id , IPC_RMID , 0) == -1) {  /* shared memory detach */
       perror("Failed to destroy shared memory segment");
       return -1;
   }
  return 0;
}
