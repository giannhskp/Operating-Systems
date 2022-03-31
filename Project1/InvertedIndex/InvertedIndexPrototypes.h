#include "InvertedIndexTypes.h"

InvertedIndex iiCreate();
InvertedIndex iiInsert(InvertedIndex ,hashNodePtr );
void iiDelete(InvertedIndex ,Student );
int iiNumber(InvertedIndex ,int );
void iiTop(InvertedIndex ,int ,int );
double iiAverage(InvertedIndex ,int );
void iiMinimum(InvertedIndex ,int );
void iiCount(InvertedIndex );
void iiPostalCode(InvertedIndex ,int );

int currentYear();

void iiDestroy(InvertedIndex );
