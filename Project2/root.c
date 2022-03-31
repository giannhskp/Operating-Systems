#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <poll.h>
#include <stdbool.h>
#include "types.h"
#include "sortlist.h"





void Usr1handler();

int signalsReceived=0;    // global variable that counts how many USR1 signals we have received from leafNodes

int	main(int argc, char* argv[]){
  int lowbound=0,upperbound=0,branchingFactor=3;  //initializing for typical reasons.
  // There is no real need to initialize beacause we check if all the parameters are read from the command line, and end the programm if they are not read.
  // But we do it so the compliler dont warn us.
  if(argc!=7){
    printf("Wrong input\n");
    return 0;
  }else{
    int count=1;
    int inputRead=0;    // for wrong input check
    while(count<=5){    // read command line parameters
      if(strcmp("-l",argv[count])==0){
        lowbound=atoi(argv[count+1]);
        inputRead++;
        count+=2;
        continue;
      }
      if(strcmp("-u",argv[count])==0){
        upperbound=atoi(argv[count+1]);
        inputRead++;
        count+=2;
        continue;
      }
      if(strcmp("-w",argv[count])==0){
        branchingFactor=atoi(argv[count+1]);
        inputRead++;
        count+=2;
        continue;
      }
    }
    if(inputRead<3){
      printf("Wrong input\n");
      return 0;
    }
  }
  struct sigaction signalAction;  // handling the signals that we receive using sigaction
  signalAction.sa_handler = Usr1handler;  // set our function as the handler (implemented at the bottom of the file)
  sigemptyset(&signalAction.sa_mask);   //initialize
  signalAction.sa_flags = SA_RESTART;
  sigaction(SIGUSR1, &signalAction, NULL);  // handle the USR1 signals

  long double totalTimes[branchingFactor*branchingFactor]; // in this board will be stored the total time of each leafNode (worker node)
  for(int i=0;i<branchingFactor*branchingFactor;i++){
    totalTimes[i]=0.0;    //initialize (for typical reasons)
  }



  int fdboard[branchingFactor][2];  // a board that contains every desctiptor for the pipes that will be created for each child
  // for every child a different pipe is created

  if((upperbound-lowbound+1)<(branchingFactor*branchingFactor)){
    printf("Give bigger range or smaller NumOfChilds\n");
    return 0;
  }

  int length= (upperbound-lowbound+1)/branchingFactor;  // the length of the range that every child (leaf) will search

  for(int i=1;i<=branchingFactor;i++){  // loop that creates the childs (innerNodes), in every loop a child is created
    int lb=lowbound+(i-1)*length;   // find the low bound for this certains child
    int ub;
    if(i<branchingFactor)   // find the upper bound for this certains child
       ub=lowbound+(i*length)-1;
    else
      ub=upperbound;

    if ( pipe ( fdboard[i-1] ) == -1 ){   // create the pipe for this certain child
      perror (" failed to create pipe ") ; exit (23) ;
    }

    int pid=fork();

  	if (pid==0){
      char *buffer[]={"./middleNodes",NULL};    //convert the name of the executable into a string so as to be given as an argument to execl
      char argId[BUFFSIZE];
      sprintf(argId,"%d",i);        //convert the leaf node's id to string to be given as a parameter to the middleNode executable
      char argBf[BUFFSIZE];
      sprintf(argBf,"%d",branchingFactor);    //convert branchingFactor to string to be given as a parameter to the middleNode executable
      char pipeFd[BUFFSIZE];
      sprintf(pipeFd,"%d",fdboard[i-1][WRITE]);   //convert pipe descriptor to to string to be given as a parameter to the middleNode executable
      char lowb[BUFFSIZE];
      sprintf(lowb,"%d",lb);    //convert lowbound to string to be given as a parameter to the middleNode executable
      char upb[BUFFSIZE];
      sprintf(upb,"%d",ub);   //convert upperbound to string to be given as a parameter to the middleNode executable
      dup2(fdboard[i-1][WRITE],1);  // duplicate the pipe
      int err=execl(buffer[0],buffer[0],argId,argBf,pipeFd,lowb,upb,(char *)NULL);  // call the middleNode executable with the above parameters
      printf("%d\n",err );
  	}else{
          // if father
          // do nothing
    }
  }
  // when all child processes are created and running
  int ended[branchingFactor]; // flag board that contains if the reading from every pipe if finished
  // if in every place of the board exists 1 that means we have finished reading from all pipes
  struct pollfd pfds[branchingFactor];  // use poll to monitor all the pipes
  // each place of the poll board corresponds to the pipe of the equivelent children
  for(int i=0;i<branchingFactor;i++){
    ended[i]=0;   // initialize the end board with zero as we have not started reading yet
    pfds[i].fd=fdboard[i][READ];  // add the pipe descriptor of the equivelent pipe to the poll board
    pfds[i].events=POLLIN;      // we are only intersted in reading so we add only POLLIN in the inrested events of poll
  }

  list l=createList();  // create a list that all the prime numbers will be stored
  // the list is a sorted list so every time we insert a prime number its is placed in the right place (see sortlist.c for the implementation)
  int primeNumbersFound=0;  // counts how many prime numbers we have read
  int flag=1;
    while(flag){
      flag=0;

      int retval = poll(pfds,branchingFactor,0);   //call poll to check if any of the pipes is ready for reading

      if(retval ==0){   //check if we have finished reading from all the pipes
        // that is because poll may return 0 if the range is large and the children have not writen yet beacause they are still computing
        for(int i=0;i<branchingFactor;i++){   //check if all pipes have ended reading
            if(ended[i]==0)
              flag=1;   // if we have not finished reading from at least one pipe, loop until one pipe is ready for reading
                        // if we have finished reading from all the pipes the flag is 0 so the loop (and the reading) is stopped
        }
        continue;
      }
      if(retval<0){ //if poll is interrupted by a signal, continue and redo poll
        flag=1;
        continue;
      }

      for(int i=0;i<branchingFactor;i++){
          node buff[TRANSFERBUFFSIZE];  // each time we read a batch (a node board) of maximum size TRANSFERBUFFSIZE .
          // TRANSFERBUFFSIZE is defined in types.h and defines the size of the buffers tha transfers the prime numbers in batches. That number can be changed depending on our preference.
          if((pfds[i].revents&POLLIN)){   // if poll returns that the pipe of this child is ready for reading
            if(read ( pfds[i].fd , buff , sizeof ( buff ))==-1)    // we read the buffer
              perror ( " Read from pipe - file \n ");
            flag=1;
            int count=buff[0].x;    // buffer may contain less prime numbers than the TRANSFERBUFFSIZE.
            // So in the first position of the buffer we always store how many items the buffer contains
            // so the folowing loop always start from position 1 of the board an not position 0
            for(int cc=1;cc<count;cc++){  // for every item in the buffer that we read
                if(buff[cc].isTotalTime==1){    // if the item contains the total time of a leafNode (worker) (explained more in types.h and leafNodes.c)
                  totalTimes[buff[cc].child - 1]= buff[cc].xx;  //add the total time of this worker to the position coressponding to his id
                  // ex in totalTimes[0] is the totalTime of W0, in  totalTimes[1] the totalTime of W1 ...
                  continue;
                }
                // if it does not contain totalTime, it contains a prime number
                l=insertToList(l,buff[cc]);   // so insert the prime number to the sorted list
                primeNumbersFound++;        // increase the prime number counter

            }
            if(buff[count-1].isTotalTime==1)    // the total times are always the last elements that are sent from a child
              ended[i]=1;                           // because the numbers are sorted and total times have LONG_MAX value
        }
      }

   }
  printList(l);     // print all the prime numbers that we found and inserted in the list
  deleteList(l);      // delete (and free) the list


  int wpid,status=0;
  while (( wpid = wait(&status)) > 0);  // wait for all the childs to finish executing  //for typical reasons, all children have finished by this point

  long double min=999999.9,max=-1.0;
  for(int i=0;i<(branchingFactor*branchingFactor);i++){
    if(totalTimes[i]<min)   // find min time for workers
      min=totalTimes[i];
    if(totalTimes[i]>max)   //find max time for workers
      max=totalTimes[i];
  }
  printf("Min Time for Workers : %Lf msecs\n",min*1000);    // we multiply by 1000 because our times are in seconds and we want to print the time in milliseconds
  printf("Max Time for Workers : %Lf msecs\n",max*1000);
  printf("Num of USR1 Received : %d / %d \n",signalsReceived,branchingFactor*branchingFactor);


  for(int i=1;i<=(branchingFactor*branchingFactor);i++){
    printf("Time for W%d:  %Lf msecs \n",i,totalTimes[i-1]*1000);
  }

  printf("    ** Prime numbers found: %d\n",primeNumbersFound);
  return 0;
}

void Usr1handler(int signum){
    signalsReceived++;    // every time we receive a USR1 signal we increase the counter
}
