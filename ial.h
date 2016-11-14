#ifndef IAL_H
#define IAL_H
#include <stdbool.h>
//#include "scaner.h"

#define TABLE_SIZE 100

typedef enum{
   TYPE_INT = 300,
   TYPE_DOUBLE = 301,
   TYPE_STRING = 302,
   TYPE_VOID = 303,
} varType;



typedef struct funcParam
{
   char *id;
   varType type;
   struct funcParam *nextParam;
} tFuncParam;

typedef struct 
{
   char *name;
   unsigned paramCnt;
   varType retType;
   tFuncParam *param;
} tFunc;

typedef struct 
{
   char *id;
   varType type;
   bool init;
} tVar;


typedef struct tList{
   
   char *key;
   bool func;
   void *data;
   struct tList *next;  
} tList;


typedef struct tList* table[TABLE_SIZE];

typedef struct 
{
   char *name;
   table data;
} tClass;


int hashFunction(const char *key, unsigned htab_size);

/**
* \brief vrati delku retezce
* \param s retezec
* \return [INT] pocet znaku
*/
int length(char *s);

char *substr(char *s, int i, int n);
int compare(char *s1, char *s2);
int find(char *s, char *search);
char *sort(char *s);

void tsInit(table *TS);

int tsInsert ( table* TS, char *key, void *data ) ;
tList* tsSearch ( table* TS, char *key );
void* tsRead ( table* TS, char *key);



#endif // IAL_H