#define  PERM 		0644
#define BUFFSIZE 1024
#define MAX_PATH_SIZE 256

typedef struct listNode *list;

void copyfile(char *,char *);
void traverseDir(char *,char *);
void checkForDeletedFiles(char *,char * );

list createList();
char *getDestPath(list );
void deleteList(list );
int checkHardLink(char *,char * );
