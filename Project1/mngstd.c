#include "mngstdPrototypes.h"
#include <stdio.h>
#include <string.h>


int main(int argc, char* argv[]){
  Data d=readInput(argc,argv);    //initialize data and keep them in this variable
  if(d==NULL){
    printf("Wrong Input Files. Please Give InputFile or ConfigFile\n");
    return 0;
  }
  //promt menu
  printf("________________________prompt________________________\n\n");
  printf(": ");
  char buffer[50];
  char function[5];
  if(fgets(buffer,50,stdin)==NULL){   //get everything that the user typed
    printf("Error\n");
    return -1;
  }
  sscanf (buffer,"%s \n",function);   //get only the first word (the function that he wants to call)
  while(strcmp(function,"exit")!=0){    //if it is exit, stop
    if(strcmp(function,"i")==0){    //insert
      char studentId[20],lastName[20],firstName[20];    // id, lastName and firstName have max size 20 characters
      int zip,year;
      float gpa;
      //read all the other words after the i  (all the student stats)
      if(sscanf (buffer,"%s %s %s %s %d %d %f\n",function,studentId,lastName,firstName,&zip,&year,&gpa)==7){
        insert(d,studentId,lastName,firstName,zip,year,gpa);    //call insert
      }else{    // if user doesn't give 6 data as written in scanf print error message and do nothing
        printf("Wrong input.\n");
      }

    }
    else if(strcmp(function,"l")==0){   //lookup
      char studentId[10];
      if(sscanf (buffer,"%s %s\n",function,studentId)==2){    //same logic as above
        lookup(d,studentId);
      }else{
        printf("Wrong input.\n");
      }
    }
    else if(strcmp(function,"d")==0){
      char studentId[10];
      if(sscanf (buffer,"%s %s\n",function,studentId)==2){
        delete(d,studentId);
      }else{
        printf("Wrong input.\n");
      }

    }
    else if(strcmp(function,"n")==0){
      int year;
      if(sscanf (buffer,"%s %d\n",function,&year)==2){
        number(d,year);
      }else{
        printf("Wrong input.\n");
      }
    }
    else if(strcmp(function,"t")==0){
      int num,year;
      if(sscanf (buffer,"%s %d %d\n",function,&num,&year)==3){
        top(d,num,year);
      }else{
        printf("Wrong input.\n");
      }
    }
    else if(strcmp(function,"a")==0){
      int year;
      if(sscanf (buffer,"%s %d\n",function,&year)==2){
        average(d,year);
      }else{
        printf("Wrong input.\n");
      }
    }
    else if(strcmp(function,"m")==0){
      int year;
      if(sscanf (buffer,"%s %d\n",function,&year)==2){
        minimum(d,year);
      }else{
        printf("Wrong input.\n");
      }
    }
    else if(strcmp(function,"c")==0){
      count(d);
    }
    else if(strcmp(function,"p")==0){
      int rank;
      if(sscanf (buffer,"%s %d\n",function,&rank)==2){
        postalcode(d,rank);
      }else{
        printf("Wrong input.\n");
      }
    }
    else{   //if nothing from the above is given as input
      printf("Wrong input.\n");
    }
    printf(": ");
    if(fgets(buffer,50,stdin)==NULL){   //read the next input line from user
      printf("Error\n");
      return -1;
    }
    sscanf (buffer,"%s \n",function);
  }
  printf("Exiting program...\n");
  deleteData(d);    //delete (and free) all tha data used

}
