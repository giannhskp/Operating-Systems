#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <limits.h>
#include "types.h"
#include "primeFunctions.h"




int	main(int argc, char *argv[]){
  if(argc>1){

    int id=atoi(argv[1]);       // id of the leaf node (1,2,3,... for W1,W2,....)
    int branchingFactor=atoi(argv[2]);      //  NumOfChilds
    int pipeWrite=atoi(argv[3]);          // the pipe of the innnerNode that will be used to write
    int lb=atoi(argv[4]);       // lowbound that this leafNodes will search
    int ub=atoi(argv[5]);// lowbound that this leafNodes will search
    long double time=0.0;     // time is passed to the prime function and is changed to the time that the function needed to find the prime number
    long double totalTime=0.0;    // we add all the times for every prime number to compute the total time of the leafNode
    unsigned long long int prime;
    if(id%branchingFactor==1){    // using the id of the leafNode call the corresponding prime algorithm
      prime=prime1(lb,ub,&time);
    }else if(id%branchingFactor==2){
      prime=prime2(lb,ub,&time);
    }else{
      prime=prime3(lb,ub,&time);
    }
    node buff3;   //the variable that we store prime numbers (and it's info) (see types.h)
    while(prime>0){
      if(prime==-1)   // the prime functions return -1 if they dont find a prime in the given range
        break;
      buff3.x=prime;    //store the prime found by the function
      buff3.xx=time;    // store the time needed to find this prime
      buff3.child=atoi(argv[1]);  // store the child id
      buff3.isTotalTime=0;
      buff3.isCount=0;
      totalTime+=time;    // add the time of this prime to the total time
      if ( write (pipeWrite , &buff3 , sizeof(buff3)) == -1)  //write the prime to the pipe of the inner node
          perror ( " Write to pipe - file \n ");
      if(id%branchingFactor==1){    //call again the prime function
                                  // prime function  every time returns the first prime it find so as so write it to the ppe immediately
                                  //so we call again the prime funcrion starting from the next number of the prime we found
        prime=prime1(prime+1,ub,&time);
      }else if(id%branchingFactor==2){
        prime=prime2(prime+1,ub,&time);
      }else{
        prime=prime3(prime+1,ub,&time);
      }
    }
    // after the last prime found, prime function searched the remaining range and returns -1 because no prime was found
    // it also returns the time spent seraching this last range so we add it to the total time
    totalTime+=time;  //add the time of the last call
    buff3.x=LONG_MAX;  // so as to always be the last element in the sorted list of middleNode
    buff3.xx=totalTime;
    buff3.child=atoi(argv[1]);
    buff3.isTotalTime=1;    // we use this flag to transfer the total time of this LeafNode
    buff3.isCount=0;
    if ( write (pipeWrite , &buff3 , sizeof(buff3)) == -1)  // write the totalTime of this leafNode to the pipe.
        // !!!!  the totalTime is the last item that is sent to the pipe from this leafNode
        perror ( " Write to pipe - file \n ");
    close(pipeWrite);   //close the pipe
    kill(getppid(),SIGUSR1);  // send USR1 signal to the innnerNode (that will be passed to the root node)

  }else{
    printf("wrong arguments\n");
  }
  return 0;
}
