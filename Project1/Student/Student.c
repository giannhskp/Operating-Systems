#include "StudentTypes.h"


Student createStudent(word stId,word lName,word fName,int zipcode,int y,double g){  //create a student node
  Student node=malloc(sizeof(studentNode));
  node->studentId=malloc((strlen(stId)+1)*sizeof(char));    // allocate memory to store the id
  node->lastName=malloc((strlen(lName)+1)*sizeof(char));    // allocate memory to store the last name
  node->firstName=malloc((strlen(fName)+1)*sizeof(char));   // allocate memory to store the first name
  strcpy(node->studentId,stId);       //copy the id
  strcpy(node->lastName,lName);
  strcpy(node->firstName,fName);
  node->zip=zipcode;
  node->year=y;
  node->gpa=g;
  return node;
}

void deleteStudent(Student node){
  if(node==NULL)
    return;
  free(node->studentId);
  free(node->lastName);
  free(node->firstName);
  free(node);
}

word getStudentId(Student st){
  return st->studentId;
}
word getLastName(Student st){
  return st->lastName;
}
word getFistName(Student st){
  return st->firstName;
}
int getZip(Student st){
  return st->zip;
}
int getYear(Student st){
  return st->year;
}
double getGpa(Student st){
  return st->gpa;
}
