#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <libgen.h>
#include <errno.h>
#include "include.h"

// global variables that are defined in mainQuic.c file
extern char origDirName[20];
extern char destDirName[20];
extern int checkLinks;
extern int verbose;
extern int copiedEntities;
extern int bytesCopied;
extern int fullPathGiven;
extern list hardLinkList;

list insertToList(list ,int ,char *,char *,list *);	// from file hardlinks.c
void deleteDirectory(char *);	// from file delete.c

int deepCopy(char *file1, char *file2){	// copy file of path:file1 to a new file in path:file2
	int infile, outfile;
	ssize_t nread;
	char buffer[BUFFSIZE];

	if ( (infile=open(file1,O_RDONLY)) == -1 )	//open file
		return(-1);

	if ( (outfile=open(file2, O_WRONLY|O_CREAT|O_TRUNC, PERM)) == -1){	// open/create the second file
		close(infile);
		return(-2);
	}

	while ( (nread=read(infile, buffer, BUFFSIZE) ) > 0 ){	// read from file1 and write to file2
		if ( write(outfile,buffer,nread) < nread ){
			close(infile); close(outfile); return(-3);
		}
	}
	close(infile); close(outfile);	//close both files

	if (nread == -1 )
		return(-4);
	else
		return(0);
}

char *replace_str(char *str, char *orig, char *rep){	//this function replaces orig string with rep string inside str string
	// is used for transforming the source path to the dest path when copying a softlink, so se dest softlink points to the corresponding path of destination hierarchy

	if(orig[strlen(orig)-1]=='/' && rep[strlen(rep)-1]!='/'){    // if source directory path name ends with '/' and dest dir path does not end with '/'
        strcat(rep,"/");    // add it also to dest dir so as to be "symmetrical"
  }
  if(rep[strlen(rep)-1]=='/' && orig[strlen(orig)-1]!='/'){    // if source directory path name ends with '/' and dest dir path does not end with '/'
      strcat(orig,"/");    // add it also to dest dir so as to be "symmetrical"
  }

	static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))  // check if orig exists in str
    return str;	//if it does not exists we dont have to replace anythin

  strncpy(buffer, str, p-str); // Copy characters from str start until orig start
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig)); //replace orig with rep and also add the rest of the str afterwards

  return buffer;
}

void copySymLink(char *link1,char * link2){	// this function is responsible to copy a source softlink to destination path
	struct stat 	mybuf;
	ssize_t len;
	char buf[BUFFSIZE];
	char *fullLinkPath;
	if ((len = readlink(link1, buf, sizeof(buf)-1)) != -1)	//read the source softlink
	    buf[len] = '\0';
	if(len==-1)
		return;
	char newpath[BUFFSIZE];

	//	if only one of the source,dest paths is full path and the other one is relative path
	char tempOrig[BUFFSIZE],tempDest[BUFFSIZE];
	int changedOrig=0; int changedDest=0;
	if(fullPathGiven){
		if(origDirName[0]!='/'){	//if source dir path is relative (and dest dir path is full path)
			strcpy(tempOrig,origDirName);		// store the relative path given by the user
			char localtemp[BUFFSIZE];
			if(realpath(tempOrig,localtemp)==NULL){		// temporarely convert it to full path, so as to match the dest path and so as the convertion of paths will be done correctly
				perror("realpath"); return;
			}
			strcpy(origDirName,localtemp);
			changedOrig=1;		// store that the source path has changed to full path in order to change it back to the original afterwards
		}
		if(destDirName[0]!='/'){	//if dest dir path is relative (and source dir path is full path)
			strcpy(tempDest,destDirName);		// store the relative path given by the user
			char localtemp[BUFFSIZE];
			if(realpath(tempDest,localtemp)==NULL){		// temporarely convert it to full path, so as to match the source path and so as the convertion of paths will be done correctly
				perror("realpath"); return;
			}
			strcpy(destDirName,localtemp);
			changedDest=1;		// store that the dest path has changed to full path in order to change it back to the original afterwards
		}
	}
	//

	if(fullPathGiven && buf[0]!='/'){	// case that full paths were given as arguments during execution and symlink contains "strange" relative path
		// if for some reason the source softlink contains a path like: ../sourceDir/ramdomDir/file1
		// the tranformation should be done correctly
		// !!  NOTE: this is a very "edge case" but it is handled as well
		char *tempDirName=strdup(origDirName);
		char *p=strstr(buf,basename(tempDirName));
		if(p!=NULL){	// if the relative path actually contains the source dir name (for example  ../sourceDir/ramdomDir/file1)
			char newbuff[BUFFSIZE];
			char *templink1=strdup(link1);
			char *dirctoryName=dirname(templink1); // find the path of the source directory that cointains tha softlink
			// and attach to it the path that the softlink points to
			strcpy(newbuff,dirctoryName);
			strcat(newbuff,"/");
			strcat(newbuff,buf);
			fullLinkPath=realpath(newbuff,NULL);	// canonicate that path, so as to have the full canonicated path that the softlink points to
			// replace the source directory name (that was given as argument during execution) with the destination directory name
			if(fullLinkPath!=NULL)
				strcpy(newpath,replace_str(fullLinkPath,origDirName,destDirName));
			else
				strcpy(newpath,replace_str(newbuff,origDirName,destDirName));
			free(templink1);
			free(fullLinkPath);
		}else{	// if it is a "normal" relative path (for example: ./randomFile)
			strcpy(newpath,replace_str(buf,origDirName,destDirName));
		}
		free(tempDirName);
	}else{
		strcpy(newpath,replace_str(buf,origDirName,destDirName));
	}
	//
	if(changedOrig)		// if the source directory path has temporarely changed to full path, revert it to the relative path
		strcpy(origDirName,tempOrig);
	if(changedDest)		// if the destination directory path has temporarely changed to full path, revert it to the relative path
		strcpy(destDirName,tempDest);
	//
	// so we have created the path that the destination softlink will "point" to
	if(lstat(link2,&mybuf) == -1){ // if the dest softlink does not exist
		if (errno != ENOENT){
			perror("stat"); return;
		}
		if(symlink(newpath,link2)==-1){	// create a new softlink that points to the path that we have generated above
			perror("symlink");
		}
		lstat(link2,&mybuf);
		bytesCopied+=(int)mybuf.st_size;
		copiedEntities++;
		if(verbose)
			printf("Created symlink: %s  ->  %s\n",link2,newpath);
	}else{	// if the dest softlink already exists
		if(!S_ISLNK(mybuf.st_mode)){	// check if the destination file is actually a softlink
			// if it is not delete the destination file and create a new softlink
			if(S_ISDIR(mybuf.st_mode)){	// if destination file is a directory
				deleteDirectory(link2);		// delete the directory
			}else{
				remove(link2);
			}
			if(symlink(newpath,link2)==-1){		// and create a new link pointing to the path that we have generated
				perror("symlink");
			}
			if(lstat(link2,&mybuf)==-1){
				perror("lstat");
				return;
			}
			bytesCopied+=(int)mybuf.st_size;
			copiedEntities++;
			if(verbose)
				printf("Created symlink: %s  ->  %s\n",link2,newpath);
		}
		char buf2[BUFFSIZE];
		ssize_t len2;
		if ((len2 = readlink(link2, buf2, sizeof(buf)-1)) != -1)	// read the dest softlink
		    buf2[len2] = '\0';
		if(strcmp(buf2,newpath)==0){	// if the softlink cointains the same path with the one that we have generated, then it is up to date
			printf("Up to date (symlink):  %s\n",link2);
			return;
		}else{	// if not, we have to update it
			unlink(link2);	// so delete the old softlink
			if(symlink(newpath,link2)==-1){		// and create a new link pointing to the path that we have generated
				perror("symlink");
			}
			lstat(link2,&mybuf);
			bytesCopied+=(int)mybuf.st_size;
			copiedEntities++;
			if(verbose)
				printf("Updated symlink: %s  ->  %s\n",link2,newpath);
		}
	}

}

void copyfile(char *name,char * newdestname){	// this function is called by traverseDir function in order to copy  a file/directory from source to dest
struct stat 	mybuf;
struct stat 	mybuf2;
char 		type;

	if(lstat(name, &mybuf)==-1){	// call stat for source file/directory
		return;		// if it does not exists return
	}
	switch (mybuf.st_mode & S_IFMT){		// find out the type of the file
		case S_IFLNK: type = 'l'; break;
  	case S_IFREG: type = '-'; break;
  	case S_IFDIR: type = 'd'; break;
  	default:      type = '?'; break;
  }

  if(type=='d'){	// if the file is a directory call again traverseDir (recursively) in order to check/copy all the files inside this folder
    traverseDir(name,newdestname);
    return;
  }
	if(type=='l'){	// if the file is a softlink
		if(checkLinks)	// if -l flag is given call copySymLink in order to copy the link, if not just ignore it
			copySymLink(name,newdestname);
		return;
	}
	if((int)mybuf.st_nlink>1 && checkLinks){	// if more than one files have the same inode as this file, and -l flag is given
		// that means that either this file is a hardlink or another hardlink exists that is "pointing" to this file
		// either way call checkHardLink in order to check/copy the file
		int status=checkHardLink(name,newdestname);
		if(status!= 0){ // if status!=0 the file/hardlink was copied inside checkHardLink so just return
			return;
		}
		// if status==0 then no other file exists with the same inode in the destination hierarchy so we must copy it normally
	}

	if(stat(newdestname,&mybuf2) == -1){	// if the destination file does not exist, we should create a new one and copy the source file
		if (errno != ENOENT){
			perror("stat"); return;
		}
		if(deepCopy(name,newdestname)<0){	// call deepCopy in order to copy the file
			perror("Error while copying file: ");
			return;
		}
		chmod(newdestname,mybuf.st_mode);	// change the permissions of the new file so as to be the same as the source file
		if(stat(newdestname,&mybuf2)==-1){		// get the size of the new file that we created
			perror("stat");
			return;
		}
		bytesCopied+=(int)mybuf2.st_size;	// add the bytes of the file to the total bytes that we have copied
		copiedEntities++;		// increse the total entities that we have copied
		if(verbose)
			printf("Copied: %s\n", newdestname);
	}else{		// if the destination file already exists
		if(mybuf.st_mode!=mybuf2.st_mode){	// if source file and destination file are not same type
			// this case only happens when -d flag is NOT given
			// if -d is given the "wrong type" destination file is deleted during checkForDeletedFiles call from traverseDir function
			if(S_ISDIR(mybuf2.st_mode)){	// if destination file is a directory
				deleteDirectory(newdestname);		// delete the directory
			}else if(S_ISLNK(mybuf2.st_mode)){	// if destination file is a sotflink
				unlink(newdestname);
			}else{
				remove(newdestname);
			}
			if(deepCopy(name,newdestname)<0){	// after deleting the old file, create a new copy of the source file
				printf("Error while copying file\n");
				return;
			}
			chmod(newdestname,mybuf.st_mode);
			if(stat(newdestname,&mybuf2)==-1){
				perror("stat");
				return;
			}
			bytesCopied+=(int)mybuf2.st_size;
			copiedEntities++;
			if(verbose)
				printf("Copied: %s\n", newdestname);
			return;
		}
		if(mybuf.st_size!=mybuf2.st_size){	// check if they have the same size, if they have not that means that the source file has changed.
			// so we have to copy again the source file like above
			if(deepCopy(name,newdestname)<0){
				printf("Error while copying file\n");
				return;
			}
			chmod(newdestname,mybuf.st_mode);
			if(stat(newdestname,&mybuf2)==-1){
				perror("stat");
				return;
			}
			bytesCopied+=(int)mybuf2.st_size;
			copiedEntities++;
			if(verbose)
				printf("Copied (because it was modified): %s\n", newdestname);
		}
		else if(mybuf.st_mtime<=mybuf2.st_mtime){	// check the modification time of source file is before the time of the dest file
			// if that is true, that mean that the dest file is up to date.
			// the only thing we have to check is if the dest file was a hardlink, but now the -l flag is not given
			// in this case (as discussed in piazza) the hardlink must be "converted" in to a normal copy of the source file
			if((int)mybuf.st_nlink>1 && (checkLinks==0)){
				list found;
				hardLinkList=insertToList(hardLinkList,(int)mybuf.st_ino,name,newdestname,&found);	// check if this file is a hardlink
				// if a file with same inode exists in the list, that mean that the file is a hardlink
				if(found!=NULL){	// if found!=NULL -> file is a hardlink
					char *originDestPath=getDestPath(found);
					struct stat origDestHardlink;
					if(stat(originDestPath,&origDestHardlink)==-1){ // call stat for the "original" file of the destination directory
		        perror("stat"); return;
		      }
					if((int)origDestHardlink.st_ino!=(int)mybuf2.st_ino){	// if the two dest files are not hardlinked
						if(verbose)
							printf("Up to date:  %s\n",newdestname);
						return;
					}
					if(unlink(newdestname)==-1){	// delete it
						perror("Unlink ");
						return;
					}
					if(deepCopy(name,newdestname)<0){	// and create a new copy of the source file (with new inode)
						perror("Error while copying file: ");
						return;
					}
					chmod(newdestname,mybuf.st_mode);
					if(stat(newdestname,&mybuf2)==-1){
						perror("stat");
						return;
					}
					bytesCopied+=(int)mybuf2.st_size;
					copiedEntities++;
					if(verbose)
						printf("Changed a hardlink to a normal file: %s\n", newdestname);
					return;
				}
			}
			if(verbose)
				printf("Up to date:  %s\n",newdestname);
		}else{
			// in this case the modification time of source file is after the time of the dest file
			// so that mean that the dest file needs to be updated.
			if(deepCopy(name,newdestname)<0){
				perror("Error while copying file: ");
				return;
			}
			chmod(newdestname,mybuf.st_mode);
			if(stat(newdestname,&mybuf2)==-1){
				perror("stat");
				return;
			}
			bytesCopied+=(int)mybuf2.st_size;
			copiedEntities++;
			if(verbose)
				printf("Copied (because it was modified): %s\n", newdestname);
		}
	}

}
