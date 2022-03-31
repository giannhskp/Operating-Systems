#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char *word;

typedef struct stnode{
  word studentId;   //word (char *) is used so letters can be used in studentId (e.x. sdi201800061)
  word lastName;
  word firstName;
  int zip;
  int year;
  double gpa;
}studentNode;
typedef studentNode *Student;   //pointer to student node
