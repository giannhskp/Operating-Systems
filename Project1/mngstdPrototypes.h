struct dataNode;
typedef struct dataNode *Data;
typedef char *word;


void insert(Data ,word ,word ,word ,int ,int ,double );
void lookup(Data ,word );
void delete(Data ,word );
void number(Data ,int );
void top(Data ,int ,int );
void average(Data ,int );
void minimum(Data ,int );
void count(Data );
void postalcode(Data ,int );

Data readInput(int , char* []);
void deleteData(Data );
