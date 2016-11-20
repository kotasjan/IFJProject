#ifndef IAL_H
#define IAL_H
#include <stdbool.h>
//#include "scaner.h"

#define TABLE_SIZE 100

typedef char * String;


typedef enum{
   TYPE_INT = 300,
   TYPE_DOUBLE = 301,
   TYPE_STRING = 302,
   TYPE_VOID = 303,
   TYPE_UNDEF = -1,
} tType;



typedef struct funcParam
{
   char *id;
   tType type;
   struct funcParam *nextParam;
} tFuncParam;



typedef struct 
{
   char *id;
   tType type;
   bool init;
   bool isDeclared;
} tVar;


typedef struct tList{
   
   char *key;
   bool func;
   void *dataPtr;
   struct tList *next;  

} tList;


typedef struct tList* table[TABLE_SIZE];

typedef struct stack
{
   table *table;
   struct stack *next;
} tStack;


typedef struct 
{
   char *name;
   table symbolTable;
   bool isDeclared;
} tClass;

typedef struct 
{
   char *name;
   unsigned paramCnt;
   tType retType;
   tFuncParam *param;
   tStack *funcStack;
   bool isDeclared;
} tFunc;


int hashFunction(const char *key, unsigned htab_size);

/**
* \brief vrati delku retezce
* \param s retezec
* \return [INT] pocet znaku
*/
int length(char *s);

char *substr(char *s, int i, int n);
int compare(char *s1, char *s2);
//int find(char *s, char *search);
//char *sort(char *s);


int readInt();
double readDouble();
String readString();
void print_int(int x);
void print_double(double x);
void print_string(char * x);
String sort(String s);
void qpartition(char * pole, int left, int right, int * i, int * j);
char * QuickSort(char * pole, int left, int right);
int find(String s, String search);
int BMA (char *P, char *T, char * CharJump);
void ComputeJumps(char * P, char * CharJump);


void tsInit(table *TS);

int tsInsert ( table* TS, char *key, void *data ) ;
tList* tsSearch ( table* TS, char *key );
void* tsRead ( table* TS, char *key);



#endif // IAL_H