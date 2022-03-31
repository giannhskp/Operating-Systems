typedef struct l{
    node *n;
    struct l *next;
}listNode;

typedef listNode *list;

list createList();
list insertToList(list ,node );
void printList(list );
void deleteList(list );
