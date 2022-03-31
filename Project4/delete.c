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

// global variables that are defined in mainQuic.c file
extern int verbose;
extern int checkLinks;
extern int deletedEntities;


void deleteDirectory(char *dirName){	// this functions deletes a given directory. It is called only from checkForDeletedFiles function.
	DIR 	*dp;
	char *newname;
	struct stat 	st;
	struct 	dirent *direntp;
	if ((dp=opendir(dirName))== NULL ) {	// open the given directory
		perror("opendir"); return;
	}
	while ((direntp = readdir(dp)) != NULL ) { // for every file inside the given directory
  		if (direntp->d_ino == 0 ) continue;
      //skip . and .. links
			int flag = strcmp(direntp->d_name,".") && strcmp(direntp->d_name,"..");
			if(!flag)
				continue;
			//create the path of the file
      newname=(char *)malloc(strlen(dirName)+strlen(direntp->d_name)+2);
  		strcpy(newname,dirName);
			if(newname[strlen(newname)-1]!='/')
  			strcat(newname,"/");
  		strcat(newname,direntp->d_name);
      // full path created in newname
			char type;
			if(lstat(newname, &st)==-1){	// find the type of the file
				free(newname); newname=NULL;
				continue;
			}
			switch (st.st_mode & S_IFMT){
		  	case S_IFREG: type = '-'; break;
		  	case S_IFDIR: type = 'd'; break;
		  	default:      type = '?'; break;
		  }
			if(type=='d'){	// if it is a directory, recursively call deleteDirectory in order to delete it
					deleteDirectory(newname);
		  }else{	// in every other case
				if(remove(newname)!=0){	// just delete the file
					perror("Delete file");
					return;
				}
				deletedEntities++;
				if(verbose)
					printf("Deleted: %s\n",newname);
			}
  		free(newname); newname=NULL;
  }	// after deleting every entity inside the directory
	closedir(dp);	// close the directory
	if(rmdir(dirName)<0){	// and finally delete the directory itself
		perror("RmDir");
	}
	deletedEntities++;
	if(verbose)
		printf("Deleted Directory: %s\n",dirName);
}


void checkForDeletedFiles(char *name,char * newdestname){
	// this function check if there are files in the destination folder that does not exists in the corresponding path of the source directory
	// it is called only when the -d flag is given from the user at execution
	DIR 	*dp;
	char * newname;
	char *newDest;
	struct stat 	stSource;
	struct stat 	stDest;
	struct 	dirent *direntp;
	if ((dp=opendir(newdestname))== NULL ) {	// open the destination directory
		perror("opendir"); return;
	}
	if(stat(name,&stSource) == -1){	// check if the source directory actually exists
		return;
	}
	while ((direntp = readdir(dp)) != NULL ) {		// for every file of the destination folder
  		if (direntp->d_ino == 0 ) continue;
			//create the full path of the DEST file
			newDest=(char *)malloc(strlen(newdestname)+strlen(direntp->d_name)+2);
			strcpy(newDest,newdestname);
			if(newDest[strlen(newDest)-1]!='/')
				strcat(newDest,"/");
			strcat(newDest,direntp->d_name);	// newname -> destination file path
			// full path created in newDest
			if(lstat(newDest, &stDest)==-1){	// call lstat for this file
				continue;
			}

      //create the full path of the SOURCE file
      newname=(char *)malloc(strlen(name)+strlen(direntp->d_name)+2);
  		strcpy(newname,name);
			if(newname[strlen(newname)-1]!='/')
  			strcat(newname,"/");
  		strcat(newname,direntp->d_name);	// newname -> source file path
      // full path created in newname
			int statReturn=lstat(newname,&stSource);
			if( statReturn== -1 || stDest.st_mode!=stSource.st_mode){
				if ((statReturn==-1) && (errno != ENOENT)){	//if another error happened
					continue;
				}
				// if dest file does not exist in the source directory
				// or if it exists but it is different file type (for example dest file is a normal file but source file is a directory with the same name)
				char type;
				switch (stDest.st_mode & S_IFMT){		// find the type of the dest file
          case S_IFLNK: type = 'l'; break;
			  	case S_IFREG: type = '-'; break;
			  	case S_IFDIR: type = 'd'; break;
			  	default:      type = '?'; break;
			  }
        if(type=='l'){	// if it is a softlink
					if(checkLinks || (statReturn!=-1)){ // if -l flag is given delete the link, else ignore it
	          unlink(newDest);
						deletedEntities++;
						if(verbose)
							printf("Deleted link: %s\n",newDest);
					}
      	}else if(type=='d'){	// if it is a directory, call the deleteDirectory function in order to delete the whole directory
					deleteDirectory(newDest);
			  }else{
					if((int)stDest.st_nlink>1){	// if it is a hardlink
						unlink(newDest);	//unlink it
					}
					else if(remove(newDest)!=0){	// if it a normal file, remove it
						perror("Delete file");
						return;
					}
					deletedEntities++;
					if(verbose)
						printf("Deleted: %s\n",newDest);
				}
			}
			free(newDest); newDest=NULL;
  		free(newname); newname=NULL;
  	}	// after traversing the whole dest directory
		closedir(dp);	// close the directory
}
