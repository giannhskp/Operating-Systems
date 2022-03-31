#include "mngstdTypes.h"


Data initalizeData(int htsize){
  Data d=malloc(sizeof(dataNode));    //create the node tha contains the data
  d->ht=htCreate(htsize);   // create the hashTable with the given size
  d->ii=iiCreate();       //create the InvertedIndex
  return d;
}

void deleteData(Data d){
  iiDestroy(d->ii);
  htDestroy(d->ht);
  free(d);
}



void insert(Data d,word stId,word lName,word fName,int zipcode,int y,double g){
  if(currentYear()<y){      //no student has a future insert year
    printf("Wrong Input. Student's year is bigger than the currentYear\n");
    return;
  }
  hashNodePtr hashTableStudentNode=htInsert(d->ht,stId,lName,fName,zipcode,y,g);    //insert the student in the hashTable
                                                                                  // htInsert returns the hashTable node that the student is saved, so as to be inserted to the InvertedIndex
  if(hashTableStudentNode!=NULL){   //if the student was inserted at the hashTable
    d->ii=iiInsert(d->ii,hashTableStudentNode);    //insert at the InvertedIndex the HashTable node that the student is saved
  }
}

void lookup(Data d,word studentId){
  htLookup(d->ht,studentId);      // call the lookup function from tha hashTable
}

void delete(Data d,word studentId){
  Student student=htReturnStudent(d->ht,studentId);   //find the student node so as to first  delete it from the InvertedIndex
  if(student==NULL){
    printf("- Student %s does not exist\n",studentId);
    return;
  }
  iiDelete(d->ii,student);          //  delete the node that points to the HashTable node with the given student from the InvertedIndex first
                                    // the whole student node is given as argument because we need both studentId and year to delete him
  htDelete(d->ht,studentId);     //then delete the student from the hash table using his id
  //message is printed inside the htDelete when the student is deleted
}

void number(Data d,int year){
  int result=iiNumber(d->ii,year);
  if(result==0)
    printf("- There are no student in year %d\n",year);
  if(result==1)
    printf("- %d student in year %d\n",result,year);
  if(result>1)
    printf("- %d students in year %d\n",result,year);
}

void top(Data d,int num,int year){
  iiTop(d->ii,num,year);
}

void average(Data d,int year){
  double result=iiAverage(d->ii,year);
  if(result>=0){
    printf("- %f\n",result);
  }else{
    printf("- No students enrolled in %d\n",year);
  }
}

void minimum(Data d,int year){
  iiMinimum(d->ii,year);
}

void count(Data d){
  iiCount(d->ii);
}

void postalcode(Data d,int rank){
  iiPostalCode(d->ii,rank);
}


Data readInput(int argc, char* argv[]){   //function that reads input and config files and also initializes the data
  char *inputfile;
  char *configfile=NULL;;
  int inputFileGiven=0;
  int configFileGiven=0;
  if(argc>2){   // at least 2 words must be given e.x. ( -i input.txt )
  	for(int i=1;i<argc;i++){   //for every word given
  		if(argv[i][0]=='-'){
  			if(argv[i][1]=='i'){   // if its -i

          inputFileGiven=1;   // flag that inputfile is given
          inputfile=malloc(strlen(argv[i+1])*sizeof(char)+1);
          strcpy(inputfile,argv[i+1]);    // store tha name of the file afte -i
  			}
        if(argv[i][1]=='c'){    //for configure file same as above
          configFileGiven=1;
          configfile=malloc(strlen(argv[i+1])*sizeof(char)+1);
          strcpy(configfile,argv[i+1]);
  			}
  		}
  	}
    if(inputFileGiven){
      FILE *fp=fopen(inputfile,"r");    //open the input file
      if(fp==NULL){       //check for error in the opening of the file
        perror("Error ");
        return NULL;
      }
      int lines=0;
      int ch;
      while(!feof(fp)){     //find how many lines the inputfile has so as to know the size of the hash table
        ch = fgetc(fp);
        if(ch == '\n')
        {
          lines++;
        }
      }
      Data d=initalizeData(lines);    // initialize hashTable and InvertedIndex
      rewind(fp);     // get the file pointer to the start of the file
      char buffer[50];    // keep tha every line that is read
      while(fgets(buffer, 50, fp)){   // read one whole line of the file untill the file ends
        char studentId[10],lastName[15],firstName[15];
        int zip,year;
        float gpa;
        // get all student characteristics from the line that was read
        sscanf (buffer,"%s %s %s %d %d %f\n",studentId,lastName,firstName,&zip,&year,&gpa);
        insert(d,studentId,lastName,firstName,zip,year,gpa);    //insert the student to out data bases
     }
     fclose(fp);    //after all lines ar read, close the file
     free(inputfile);   //free the word that kept inputfile name
     if(configfile!=NULL)   // if also configure file was given, we dont need it
      free(configfile);     // so delete the word that kept the name of the file
     return d;
   }else if(configFileGiven){     //if only configure file was given
     FILE *fp=fopen(configfile,"r");
     if(fp==NULL){
       perror("Error ");
       free(configfile);
       return NULL;
     }
     char buffer[50];
     int size;
     char string[10];
     Data d;
     if(fgets(buffer, 50, fp)==NULL){ //read the first line (config file contains only one line)
       free(configfile);
       return NULL;
     }
     sscanf (buffer,"%s %d\n",string,&size);    //get the word and tha size that config contains
     if(strcmp(string,"HashTableSize")==0){
       d=initalizeData(size);     //intialize data with the standard size that configure file contains
     }else{       //worng config file
       d=NULL;
     }
     fclose(fp);
     free(configfile);    // free the file name word
     return d;
   }else{     //if neither input file not configure file is given
     return NULL;
   }
 }else{   //if neither input file not configure file is given
   return NULL;
 }
 return NULL;
}
