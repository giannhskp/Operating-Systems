#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "types.h"
#include "sortlist.h"
#include <poll.h>


void Usr1handler();


void quicksort(node *,int ,int );

int	main(int argc, char *argv[]){
  struct sigaction signalAction;    // handling the signals that we receive using sigaction
  signalAction.sa_handler = Usr1handler;  // set our function as the handler (implemented at the bottom of the file)
  sigemptyset(&signalAction.sa_mask);
  signalAction.sa_flags = SA_RESTART;
  sigaction(SIGUSR1, &signalAction, NULL);  // handle the USR1 signals


  if(argc<5){
    printf("wrong arguments\n");
    exit(1);
  }

  int id=atoi(argv[1]);   // get the id of the innerNode (1,2,..,branchingFactor) for I1,I2,...
  int branchingFactor=atoi(argv[2]);    // NumOfChilds
  int fatherPipe=atoi(argv[3]);       // get the descriptor of the pipe that will be used for writing to the parent node (root)
  int lowbound=atoi(argv[4]);     // lowbound that this innerNode will search
  int upperbound=atoi(argv[5]);   // lowbound that this innerNode will search

  int fdboard[branchingFactor][2]; // a board that contains every desctiptor for the pipes that will be created for each child
  // for every child a different pipe is created


  int length= (upperbound-lowbound+1)/branchingFactor;    // the length of the range that every child (leaf) will search

  for(int i=1;i<=branchingFactor;i++){
    int lb=lowbound+(i-1)*length; // find the low bound for this certains child
    int ub;
    if(i<branchingFactor)       // find the upper bound for this certains child
       ub=lowbound+(i*length)-1;
    else
      ub=upperbound;

    if ( pipe ( fdboard[i-1] ) == -1 ){   // create the pipe for this certain child
      perror (" failed to create pipe ") ; exit (23) ;
    }

    int pid=fork();   // create the child process (leafNode) using fork
  	if (pid==0){ //if child

      char *buffer[]={"./leafNodes",NULL};  //convert the name of the executable into a string so as to be given as an argument to execl
      char childId[BUFFSIZE];
      sprintf(childId,"%d",(id-1)*branchingFactor+i);   //convert id to string to be given as a parameter to the leafNode executable
      char argBf[BUFFSIZE];
      sprintf(argBf,"%d",branchingFactor);     //convert branchingFactor to string to be given as a parameter to the leafNode executable
      char pipeFd[BUFFSIZE];
      sprintf(pipeFd,"%d",fdboard[i-1][WRITE]);   //convert pipe descriptor to to string to be given as a parameter to the leafNode executable
      char lowb[BUFFSIZE];
      sprintf(lowb,"%d",lb);  //convert lowbound to string to be given as a parameter to the leafNode executable
      char upb[BUFFSIZE];
      sprintf(upb,"%d",ub);   //convert upperbound to string to be given as a parameter to the leafNode executable
      dup2(fdboard[i-1][WRITE],1);    // duplicate the pipe
      close(fdboard[i-1][READ]);   // close the read as we only need to write in the child process
      int err=execl(buffer[0],buffer[0],childId,argBf,pipeFd,lowb,upb,(char *)NULL);    // call the leafNode executable with the above parameters
      printf("%d\n",err );
  	}else{
        //if father
        // do nothing
    }
  }
  // when all child processes are created and running
  int count=0;
  list l=createList();    // create a list that all the prime numbers will be stored
  // the list is a sorted list so every time we insert a prime number its is placed in the right place (see sortlist.c for the implementation)

  // the poll use here, follows the same logic as the root so some comments are exactly the same as the root.c file
  struct pollfd pfds[branchingFactor];    // use poll to monitor all the pipes
  // each place of the poll board corresponds to the pipe of the equivelent children
  int ended[branchingFactor];   // flag board that contains if the reading from every pipe if finished
  // if in every place of the board exists 1 that means we have finished reading from all pipes
  for(int i=0;i<branchingFactor;i++){
    ended[i]=0;   // initialize the end board with zero as we have not started reading yet
    pfds[i].fd=fdboard[i][READ];    // add the pipe descriptor of the equivelent pipe to the poll board
    pfds[i].events=POLLIN;      // we are only intersted in reading so we add only POLLIN in the inrested events of poll
  }
  int flag=1;
    while(flag){
      flag=0;

      int retval = poll(pfds,branchingFactor,10);   //call poll to check if any of the pipes is ready for reading

      if(retval ==0){ //check if we have finished reading from all the pipes
        // that is because poll may return 0 if the range is large and the children have not writen yet beacause they are still computing
        for(int i=0;i<branchingFactor;i++){
            if(ended[i]==0)
              flag=1;   //if at least one pipe has not finished, continue looping and execute again poll and reading
        }
        continue;
      }
      if(retval<0){   // poll sometimes is interrupted by the USR! signals hat are sent so we go back and execute again the poll function
        flag=1;
        continue;
      }

      for(int i=0;i<branchingFactor;i++){
          node buff3;
          if((pfds[i].revents&POLLIN)){ //check if this child has writen
            flag=1;
            //if poll returned that this pipe is ready for reading
            if(read ( pfds[i].fd , &buff3 , sizeof ( buff3 ))==-1)   // read from the pipe
              perror ( " Read from pipe - file \n ");
            l=insertToList(l,buff3);    // add the prime to the list
            count ++;   //increase the prime counter
            if(buff3.isTotalTime==1)    // total time is always the last object that is sent from a leafNode
              ended[i]=1;   // so if we read totalTime that means that we read the last item that was sent and therfore we finished reading from this pipe
        }
      }
   }

  node buff[count]; // create a board that will contain all the primes we found (the size of the board is exactly the number of primes we found)
  for(int i=0;i<count;i++){
    if(l==NULL)
      break;
    buff[i]= *(l->n);
    l=l->next;
  }
  deleteList(l);    // free up the list

  int tempcount=count;  // the total numbers of prime that he have to write
  int loopnum=0;
  // we need to write the prime number in batches
  // the size of each batch is defined in types.h in the variable TRANSFERBUFFSIZE
  // metaphorically we need to "break" the board that contains all the primes in to smaller TRANSFERBUFFSIZE sized boards and write each board to the pipe
  while(tempcount>0){
     if(tempcount<0)
        break;
     if(tempcount>TRANSFERBUFFSIZE){  // if we have more than TRANSFERBUFFSIZE primes, fill in a batch and send it
       tempcount-=(TRANSFERBUFFSIZE-1);
       node tempbuff[TRANSFERBUFFSIZE];   // a batch of nodes (primes)
       node countnode;
       countnode.x=TRANSFERBUFFSIZE;
       countnode.isCount=1;
       tempbuff[0]=countnode; // in the first place of tha batch is always a node that only contains how many nodes the batch actually has
       for(int i=1;i<TRANSFERBUFFSIZE;i++){
         tempbuff[i]=buff[(loopnum*(TRANSFERBUFFSIZE-1))+i-1];    // copy the primes in to the buffer
       }
       if ( write (fatherPipe , tempbuff , sizeof(tempbuff)) == -1)   // write the batch to the pipe
           perror ( " Write to pipe - file \n ");
     }else{   // if less than TRANSFERBUFFSIZE primes are remaining (the final batch that will be sent)
       // same logic as above
       node tempbuff[TRANSFERBUFFSIZE];
       node countnode;
       countnode.x=tempcount+1;
       countnode.isCount=1;
       tempbuff[0]=countnode;   // is critical for the receiver to know how many nodes exactly the batch contains as is has fewer than the board size
       for(int i=1;i<=tempcount;i++){
         tempbuff[i]=buff[(loopnum*(TRANSFERBUFFSIZE-1))+i-1];
       }
       if ( write (fatherPipe , tempbuff , sizeof(tempbuff)) == -1)
           perror ( " Write to pipe - file \n ");
        tempcount-=(TRANSFERBUFFSIZE-1);
     }
     loopnum++;
   }
    close(fatherPipe);    // close the pipe that we wrote on
    int wpid,status=0;
    while (( wpid = wait(&status)) > 0);    //wait for every child to finish  // only for typical reasons, all children are always ended by this point

  return 0;
}


void Usr1handler(){
    kill(getppid(),SIGUSR1);    // if we receive a USR1 sugnal from the child send it to the root
}
