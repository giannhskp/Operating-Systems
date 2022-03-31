#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include	<sys/file.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include	<errno.h>
#include 	<assert.h>
#include <sys/time.h>
#include <sys/times.h>  /* times() */

#define MUTEX_SEMAPHORE_NAME "/semaphoreName"
#define TABLE_EMPTY_SEMAPHORE_NAME "/tableEmptySemaphoreName"
#define TABLE_FULL_SEMAPHORE_NAME1 "/tableFullSemaphoreName1"
#define TABLE_FULL_SEMAPHORE_NAME2 "/tableFullSemaphoreName2"
#define TABLE_FULL_SEMAPHORE_NAME3 "/tableFullSemaphoreName3"
#define END_SEMAPHORE_NAME "/endSemaphoreName"
#define JOIN_LOG_SEMAPHORE_NAME "/joinedLogFileSemaphoreName"
#define ONION 0
#define GREEN_PEPPER 1
#define TOMATO 2
#define TRUE 1
#define FALSE 0
#define FILENAME_SIZE 50


typedef struct sm{
  int saladsToBeMade;
  int finished[3];
  int saladsMade[3];
  int pid[3];
}smData;
