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
#include "include.h"


int checkLinks;		// contains 1 if -l flag was given as argument at execution and 0 if not
int deleteFlag;		// contains 1 if -d flag was given as argument at execution and 0 if not
int verbose;			// contains 1 if -v flag was given as argument at execution and 0 if not
char origDirName[MAX_PATH_SIZE];		// contains the source file name/path that was given as argument at execution
char destDirName[MAX_PATH_SIZE];		// contains the destination file name/path that was given as argument at execution
list hardLinkList;					// a list that is used for managing hardlink
int totalEntities;		// counts how many entities are seen while travensing the directory hierarchy
int copiedEntities;		// counts how many entities are actualy copied
int bytesCopied;		// counts how many bytes were copied
int deletedEntities;		// counts how many entities were deleted from the destination directory
int fullPathGiven;	// contains 1 if the full paths were given as arguments at execution and 0 otherwise (used in function copySymLink in file copy.c)



void traverseDir(char *name,char *destName){		// this is the main function used for copying the source directory
	//we recursively traverse the directories in the source hierarchy
	DIR 	*dp;
	char * newname;
	char * newdestname;
	struct stat 	st;
	struct stat 	currDir;
	struct 	dirent *direntp;

	if(stat(name,&currDir) == -1){	//check if the source directory actually exists
		return;
	}
	if ((dp=opendir(name))== NULL ) {	// open the directory
		perror("opendir"); return;
	}
	if(stat(destName,&st) == -1){	 // if the corresponding destination directory does not exist
		if (errno != ENOENT){
			perror("stat"); return;
		}
		mkdir(destName,currDir.st_mode);	//create a new directory at the destination path with the same name and same permissions
		copiedEntities++;
		if(verbose)
			printf("Created a directory: %s\n",destName);
	}else{	// if a directory/file with the same name already exists in the destination path
		if(deleteFlag){	// if -d flag was given at execution
			checkForDeletedFiles(name,destName);	// call this function in order to delete files that exists in destination folder but not in the source folder
		}else{	// if -d flag was not given
			if(!S_ISDIR(st.st_mode)){	// check if the file in the destination path is actually a directory
				// if it is not a directory, delete the file and create a new directory with the same name
				// NOTE: if -d flag is given, this "job" is done inside checkForDeletedFiles function
				remove(destName);
				mkdir(destName,currDir.st_mode);
				copiedEntities++;
				if(verbose)
					printf("Replaced: %s  with a new directory\n",destName);
			}
		}
	}
	/// checking if by copying this directory a cycle is created
	// we are checking if the starting destination directory is inside the starting source directory
	// in reality, every time we check if the currrent source directory that we are about to copy has the same path as the given destination path (that was given at execution)
	// if a cycle is created we skip copying this folder and we continue with the rest files of the hierarchy
	char *tempath1=realpath(name,NULL);
	char *tempath2=realpath(destDirName,NULL);
	if(strcmp(tempath1,tempath2)==0){
		if(verbose)
			printf("** By copying: %s a cycle is created, so it is skipped\n",name);
		free(tempath1);
		free(tempath2);
		closedir(dp);
		return;
	}
	free(tempath1);
	free(tempath2);
	///
	while ((direntp = readdir(dp)) != NULL ) {	// for every file of the source directory
  		if (direntp->d_ino == 0 ) continue;
      //create the full path of the source file (directoryName + fileName)
      newname=(char *)malloc(strlen(name)+strlen(direntp->d_name)+2);
  		strcpy(newname,name);
			if(newname[strlen(newname)-1]!='/')
  			strcat(newname,"/");
  		strcat(newname,direntp->d_name);
      // full path created in newname
			//create the full path of the destination file (directoryName + fileName)
      newdestname=(char *)malloc(strlen(destName)+strlen(direntp->d_name)+2);
  		strcpy(newdestname,destName);
			if(newdestname[strlen(newdestname)-1]!='/')
  			strcat(newdestname,"/");
  		strcat(newdestname,direntp->d_name);
      // full path created in newdestname
			// if the file is the . or the .. link, skip it
      int flag = strcmp(direntp->d_name,".") && strcmp(direntp->d_name,"..");
			if(flag!=0){
				totalEntities++;
				copyfile(newname,newdestname); // call the function in order to copy the file/directory
			}
      //
  		free(newname); newname=NULL;
			free(newdestname); newdestname=NULL;
			// continue with the next file/directory that is inside the source directory
  }
	closedir(dp);	// after traversing and calling copyfile fall all the "items" of the directory, close it
}

void fixPath(char *, char );

int	main(int argc, char *argv[]){
	if(argc<3){
		printf("Wrong input\n");
		return -1;
	}
	struct stat mybuf;
	// initialize all the global variables/flags
	checkLinks=0;
	deleteFlag=0;
	verbose=0;

	totalEntities=1;	//starts from 1 so as to count the given directory
	copiedEntities=0;
	bytesCopied=0;
	fullPathGiven=0;
	// initialize the list that will be used when copying files/hardlinks
	hardLinkList=createList();
	clock_t begin = clock();
	// read command link arguments (flags)
	for(int i=1;i<(argc-2);i++){
		if(strcmp(argv[i],"-l")==0){
			checkLinks=1;
		}else if(strcmp(argv[i],"-v")==0){
			verbose=1;
		}else if(strcmp(argv[i],"-d")==0){
			deleteFlag=1;
		}else{
			printf("Wrong input\n");
			return -1;
		}
	}
	//
  if (stat(argv[argc-2], &mybuf) < 0) {	//chech if the source file actually exists
		perror(argv[argc-2]);
		return -1;
	}

  if ((mybuf.st_mode & S_IFMT) == S_IFDIR ){	// if the source is a directory
			// canonicate paths
			fixPath(argv[argc-2],'o');
			fixPath(argv[argc-1],'d');
			// and start the quic process by calling the traverseDir function
    	traverseDir(origDirName,destDirName);
	}else{	// if the source is not a directory, just copy the file to the dest path
		totalEntities++;
		fixPath(argv[argc-2],'o');
		fixPath(argv[argc-1],'d');
		copyfile(origDirName,destDirName);
	}

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	//the quic process has ended, so we must print the statistics that are stored at the global variables
	printf("There are %d files/directories in the hierarchy\n",totalEntities);
	printf("Number of entities copied is %d\n", copiedEntities);
	printf("Copied %d bytes in %f seconds at %f bytes/sec\n", bytesCopied,time_spent,bytesCopied/time_spent);
	if(deleteFlag)
		printf("Number of entities deleted from destination directory is %d\n", deletedEntities);
	// add total time and byte/sec
	deleteList(hardLinkList);	// finally delete/free the linked list we used
	return 0;
}

void fixPath(char *oldPath,char which){	// this function canonicates the given path
	// if a path in this form is given: ./exampleDir it is tranformed into: exampleDir
	// also this function identifies if a full or relative path is given. Depending on that the fullPathGiven flag variable is set on 1 or 0.
	if(oldPath[0]=='.' && oldPath[1]=='/'){	// if the given path needs to be canonicated
		if(which=='o')		// if the given path is the source path
			strcpy(origDirName,&oldPath[2]);	// delete the first two characters ./ , and store the path to the global variable origDirName
		else if (which=='d')	// if the given path is the destination path
			strcpy(destDirName,&oldPath[2]);
	}else{		// if the path does not need to be canonicated, just copy it as it is in the global variables
		if(which=='o')
			strcpy(origDirName,oldPath);
		else if (which=='d')
			strcpy(destDirName,oldPath);
		if(oldPath[0]=='/')		// if the path starts with the / character, then it is a full path
			fullPathGiven=1;
	}
}
