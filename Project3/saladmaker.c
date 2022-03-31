#include "header.h"

int main(int argc, char *argv[]){
  time_t t;
  int pid=getpid();   // process id
  int smId,ingredient,lb,ub;
  if(argc!=9){
    printf("Wrong input\n");
    return 0;
  }else{  //read command line arguments
    int count=1;
    int inputRead=0;    // for wrong input check
    while(count<=7){
      if(strcmp("-t1",argv[count])==0){   // lowbound for job time
        lb=atoi(argv[count+1]);
        inputRead++;
        count+=2;
        continue;
      }
      if(strcmp("-t2",argv[count])==0){ // upperbound for job time
        ub=atoi(argv[count+1]);
        inputRead++;
        count+=2;
        continue;
      }
      if(strcmp("-s",argv[count])==0){
        smId=atoi(argv[count+1]);
        inputRead++;
        count+=2;
        continue;
      }
      if(strcmp("-id",argv[count])==0){ // saladmaer id (1 for saladmaker1, 2 for salamaker2 and 3 for saladmaker2)
        if(atoi(argv[count+1])==1 || atoi(argv[count+1])==2 || atoi(argv[count+1])==3)    // check if correct id is givven
          ingredient=atoi(argv[count+1])-1;
        else{
          printf("Wrong saladmaker id\n");
          return 0;
        }
        inputRead++;
        count+=2;
        continue;
      }
    }
    if(inputRead<4){    // check if all arguments were read
      printf("Wrong input\n");
      return 0;
    }
  }


  // log file of the saladmaker
  char fileName[FILENAME_SIZE]="logFile_saladmaker";    // create the file name depending on the shared memory id
  char tempBuff[3];
  sprintf(tempBuff, "%d", ingredient+1);
  strcat(fileName, tempBuff);
  strcat(fileName, "_");
  char tempBuff2[15];
  sprintf(tempBuff2, "%d", smId);
  strcat(fileName, tempBuff2);
  strcat(fileName, ".txt");
  FILE *logFile=fopen (fileName, "a");  // create/open the file
  if(logFile==NULL){
    perror("Error");
    return -1;
  }
  //
  //   log file that is used for finding the common time intervals
  char fileName2[FILENAME_SIZE]="activeTimesLogFile_";  // create the file name depending on the shared memory id
  char fileNametemp2[10];
  sprintf(fileNametemp2, "%d", smId);
  strcat(fileName2, fileNametemp2);
  strcat(fileName2, ".txt");
  FILE *activeTimesLogFile=fopen (fileName2, "a");  // create/open the file
  if(activeTimesLogFile==NULL){
    perror("Error");
    return -1;
  }
  //
  //    joined logFile
  char fileName3[FILENAME_SIZE]="joinedLogFile_"; // create the file name depending on the shared memory id
  char fileNametemp3[10];
  sprintf(fileNametemp3, "%d", smId);
  strcat(fileName3, fileNametemp3);
  strcat(fileName3, ".txt");
  FILE *joinedLogFile=fopen (fileName3, "a"); // create/open the file
  if(joinedLogFile==NULL){
    perror("Error");
    return -1;
  }
  //
  smData *smLoc;
  if ((smLoc = (smData *)shmat(smId, NULL, 0)) == (void *)-1) {   // attach the shared memory that is created (and initialized) by the chef
      perror("Failed to attach memory segment");
      return 1;
  }
  printf("Saladmaker%d successfully attached the shared memory segment with id %d\n",ingredient+1,smId);
  //
  sem_t *mutex=sem_open(MUTEX_SEMAPHORE_NAME, 0);
  // mutex semaphore is use to acheive mutual exclusion while processes access (and read or modify) the shared memory
  if(mutex == SEM_FAILED){
    perror("Failed to open semaphore on saladmaker");
    return -1;
  }
  sem_t *tableFullSemaphore;    // this semaphore is used so as the saladmaker is notified when chef has selected saladmaker's ingeedients (and placed them in the table)
  // open the correct semaphore depending on the saladmaker id. One tableFullSemaphore semaphore exists for each saladmakers
  if(ingredient==0)
    tableFullSemaphore=sem_open(TABLE_FULL_SEMAPHORE_NAME1, 0);
  else if(ingredient==1)
    tableFullSemaphore=sem_open(TABLE_FULL_SEMAPHORE_NAME2, 0);
  else
    tableFullSemaphore=sem_open(TABLE_FULL_SEMAPHORE_NAME3, 0);
  if(tableFullSemaphore == SEM_FAILED){ // check for error while opening semaphore
    perror("Failed to open semaphore on saladmaker");
    return -1;
  }
  //
  sem_t *tableEmptySemaphore=sem_open(TABLE_EMPTY_SEMAPHORE_NAME, 0);
  // tableEmptySemaphore is showing if the table is full of ingredient (= the corresponding saladmaker has not yet got his ingredients)
  // if table is not empty chef is susspended on tableEmptySemaphore until the saladmaker gets his ingredients.
  // so when a saladmaker gets his ingredients from the table, he posts the semaphore so as to "wake up" the chef
  if(tableEmptySemaphore == SEM_FAILED){
    perror("Failed to open semaphore on saladmaker");
    return -1;
  }
  //
  sem_t *endSemaphore=sem_open(END_SEMAPHORE_NAME, 0);
  // end semaphore is used to notify the chef that all salads were made
  if(endSemaphore == SEM_FAILED){
    perror("Failed to open semaphore on saladmaker");
    return -1;
  }
  //
  sem_t *joinLogSemaphore=sem_open(JOIN_LOG_SEMAPHORE_NAME, 0);
  // this semaphore is used so as to ensure that only one process is writing at a time at the joined logFile
  if(joinLogSemaphore == SEM_FAILED){
    perror("Failed to open semaphore on saladmaker");
    return -1;
  }
  //

  sem_wait(mutex);  // mutex is used because we are accessing shared memory
  // if another process is already accessing shared memory the second process is suspended on mutex semaphore until the other process posts mutex semaphore
    smLoc->pid[ingredient]=pid; // store the process id (pid)
  sem_post(mutex);  // post mutex semaphore as we are done accessing shared memory for now
  int flag=TRUE;

  while(flag){
    // when a saladmaker just ended making a salad, he checks if all salads are made
    sem_wait(mutex);  //accessing shared memory
    if(smLoc->saladsToBeMade<=0){   // check if all salads are made
      flag=FALSE;   // if so, set flag as False so as the while loop stops
      smLoc->finished[ingredient]=TRUE;   // assign at shared memory that i am about to end
      sem_post(mutex);  //finished accessing shared memory
      continue; // continue so as the next instruction to execute is the while statement and therefore, the loop stops
    }
    sem_post(mutex);  //finished accessing shared memory
    ///
    time_t T= time(NULL);   //get current time
    struct  tm currtime = *localtime(&T);
    struct timeval  nn;
    gettimeofday(&nn, NULL);
    // write on the personal logFile. No need for semaphore because this process is the only one writing on this file
    fprintf(logFile,"[%02d:%02d:%02d:%04d] [%d] [Saladmaker%d]  Waiting for ingredients\n",currtime.tm_hour, currtime.tm_min, currtime.tm_sec,(int)(nn.tv_usec / 1000),pid,ingredient+1);
    sem_wait(joinLogSemaphore); // ensure that no one is writing at the joinedLogFile
      // write on the joinedLogFile
      fprintf(joinedLogFile,"[%02d:%02d:%02d:%04d] [%d] [Saladmaker%d]  Waiting for ingredients\n",currtime.tm_hour, currtime.tm_min, currtime.tm_sec,(int)(nn.tv_usec / 1000),pid,ingredient+1);
      fflush(joinedLogFile);     // empty the buffer so as the line will imediatelly be writen on the file
    sem_post(joinLogSemaphore);

    ////
    sem_wait(tableFullSemaphore); // wait until the chef notifies this saladmaker (to get his ingredients from the table)
    // the SaladMaker process is suspended on this semaphore until the chef notifies
    ////
        // directly after "waking up" check if all salads are made, there for saladmaker does not have to get ingredient and has to end the process
        // this case happens for the salad makers that didn't create the last ended salad and chef woke them up so as to finish
        // see also the file chef.c at lines: 251-257
        sem_wait(mutex);  //accessing shared memory
          if(smLoc->saladsToBeMade<=0){ // if salads are made, end (same as above)
            flag=FALSE;
            smLoc->finished[ingredient]=TRUE;
            sem_post(mutex);
            sem_post(tableFullSemaphore);
            continue;
          }
        sem_post(mutex);  //finished accessing shared memory

      // Salad maker gets his ingeedients
        T= time(NULL);  // get current time
        struct  tm fetchTime = *localtime(&T);
        struct timeval  fetchNow;
        gettimeofday(&fetchNow, NULL);
        // write on the personal logFile. No need for semaphore because this process is the only one writing on this file
        fprintf(logFile,"[%02d:%02d:%02d:%04d] [%d] [Saladmaker%d] Got ingredients\n",fetchTime.tm_hour, fetchTime.tm_min, fetchTime.tm_sec,(int)(fetchNow.tv_usec / 1000),pid,ingredient+1);
        sem_wait(joinLogSemaphore); // ensure that no one is writing at the joinedLogFile
          // write on the joinedLogFile
          fprintf(joinedLogFile,"[%02d:%02d:%02d:%04d] [%d] [Saladmaker%d] Got ingredients\n",fetchTime.tm_hour, fetchTime.tm_min, fetchTime.tm_sec,(int)(fetchNow.tv_usec / 1000),pid,ingredient+1);
          fflush(joinedLogFile);
        sem_post(joinLogSemaphore);

        T= time(NULL);
        struct  tm startTime = *localtime(&T);
        struct timeval  startNow;
        gettimeofday(&startNow, NULL);
        srand((unsigned) time(&t));
        //
        // remove time needed from print
        //
    // saladaker starts making the salad
        int cuttingTime=(rand()%(ub-lb+1))+lb;
        fprintf(logFile,"[%02d:%02d:%02d:%04d] [%d] [Saladmaker%d] Start making salad   -- Time needed %d secs --\n",startTime.tm_hour, startTime.tm_min, startTime.tm_sec,(int)(startNow.tv_usec / 1000),pid,ingredient+1,cuttingTime);
        sem_wait(joinLogSemaphore); // ensure that no one is writing at the joinedLogFile
          // write on the joinedLogFile
          fprintf(joinedLogFile,"[%02d:%02d:%02d:%04d] [%d] [Saladmaker%d] Start making salad   -- Time needed %d secs --\n",startTime.tm_hour, startTime.tm_min, startTime.tm_sec,(int)(startNow.tv_usec / 1000),pid,ingredient+1,cuttingTime);
          fflush(joinedLogFile);
        sem_post(joinLogSemaphore);

    sem_post(tableEmptySemaphore);  // notify the chef that got the ingredients and the table is now empty

    sleep(cuttingTime); // cut the salad
    // made the salad
    sem_wait(mutex);  //accessing shared memory
      smLoc->saladsToBeMade--;  // decrease the remaining salads by one
      smLoc->saladsMade[ingredient]++;  // add one salad to the salads made by this saladmaker
      T= time(NULL);
      struct  tm endTime = *localtime(&T);
      struct timeval  endNow;
      gettimeofday(&endNow, NULL);
      // add the active time interval to the activeTimesLogFile so as to compute the common time intervals afterwards
      // only one salad maker writes to the activeTimesLogFile due to mutex semaphore
      fprintf(activeTimesLogFile,"SALADMAKER%d %02d:%02d:%02d:%03d-%02d:%02d:%02d:%03d\n",ingredient+1,startTime.tm_hour, startTime.tm_min, startTime.tm_sec,(int)(startNow.tv_usec / 1000),endTime.tm_hour, endTime.tm_min, endTime.tm_sec,(int)(endNow.tv_usec / 1000));
    sem_post(mutex);  //finished accessing shared memory
    fprintf(logFile,"[%02d:%02d:%02d:%04d] [%d] [Saladmaker%d] End making salad\n",endTime.tm_hour, endTime.tm_min, endTime.tm_sec,(int)(endNow.tv_usec / 1000),pid,ingredient+1);
    sem_wait(joinLogSemaphore); // ensure that no one is writing at the joinedLogFile
      // write on the joinedLogFile
      fprintf(joinedLogFile,"[%02d:%02d:%02d:%04d] [%d] [Saladmaker%d] End making salad\n",endTime.tm_hour, endTime.tm_min, endTime.tm_sec,(int)(endNow.tv_usec / 1000),pid,ingredient+1);
      fflush(joinedLogFile);
    sem_post(joinLogSemaphore);
    // after writing the logs go to the start of the loop to check if all salads are made, or else to continue with the same steps
  }
  printf("Saladmaker%d ended\n",ingredient+1);
  sem_post(endSemaphore);
  if (shmdt((void *)smLoc) == -1) {  /* shared memory detach */
       perror("Failed to destroy shared memory segment");
       return 1;
   }
   // close open files
   fclose(activeTimesLogFile);
   fclose(joinedLogFile);
   fclose(logFile);
   // semaphores dont need to be closed because they are closed afterwards by the chef
  return 0;
}
