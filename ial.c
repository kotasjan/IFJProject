#include "ial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int length(char *s) {

	return strlen(s);

}

char *substr(char *s, int i, int n) {

	if( strlen(s) < (i + n) )
		return NULL;

	char *str = malloc( (n + 1) * sizeof(char) );
	if(!str)
		return NULL;

	return memcpy( str, s + i, n );

}

int compare(char *s1, char *s2) {

    int i = 0;
    while ( s1[i] != '\0' ) {
    	
        if( s2[i] == '\0' ) return 1; 
        else if( s1[i] > s2[i] ) return 1; 
        else if( s1[i] < s2[i] ) return -1; 

        i++;
    } 

    return 0;

}

int find(char *s, char *search) {

}

char *sort(char *s) {
	
}

int readInt() {

}

int hashFunction(const char *key, unsigned htab_size)
{
    int retval = 1;
    int keylen = strlen(key);
    for ( int i=0; i<keylen; i++ )
        retval += key[i];
    return ( retval % htab_size );
}

void tsInit(table *TS)
{

    if((*TS) == NULL)
        return ;
    for(int i = 0; i<TABLE_SIZE ;i++)
    {
        ((*TS)[i]) = NULL;
    }
    return;

}

tList* tsSearch ( table* TS, char *key ) {
    
    if((*TS) == NULL) return NULL;
    int hash = hashFunction(key, TABLE_SIZE);
    tList * tmp = (*TS)[hash];
    if(tmp)
    {
        while (tmp)
        {
            if(!(strcmp(tmp->key, key)))
                return tmp;
            else
                tmp = tmp->next;
        }
    }
    return NULL;
}

int tsInsert ( table* TS, char *key, void *data ) {

    tList *tmp = tsSearch(TS, key); 
    if(tmp) 
        return 2;
    else 
    {
        tmp = calloc(1, sizeof(tList));
        if(!tmp)
            return 1;
        tmp->key = key;
        tmp->data = data; 
        int hash = hashFunction(key, TABLE_SIZE);
        if((*TS)[hash])
        {
            tmp->next = (*TS)[hash];
        }
        else
            tmp->next = NULL;

        (*TS)[hash] = tmp;            
        return 0;
    }
}

void* tsRead ( table* TS, char *key ) {

   tList *item = tsSearch (TS, key);
   if(!item) 
      return NULL;
   else
   {
      return item;
   }
}


#include "debug.h"
void printTSa(table *TS)
{
   // 100 = TABLE_SIZE
   tList *item;
   for(int i = 0; i < 100; i++)
   {
      if((*TS)[i])
      {
         item = (*TS)[i];
         while(item)
         {
            printf("CLASS: %s\n", (*TS)[i]->key);
            tClass *trida = item->data;
            printf("tttt %s\n",trida->name);
            table *class = trida->data;
            for(int j = 0; j < 100; j++)
            {

               if((*class)[j])
               {
                  if((*class)[j]->func){
                     printf("Function: %s\n", (*class)[j]->key);
                     printFunction((*class)[j]->data);
                  }
                  else
                  {
                     tVar *var = (*class)[j]->data;
                     debugVar("Variable Name: %s\n", var->id);
                     debugVar("Variable type: %s\n", var->type == 300 ? "int" : var->type == 301 ? "double" : var->type == 302 ? "string" : "void" );
                     debugVar("Variable init: %s\n", var->init ? "true" : "false");
                  }

               }
            }
            item = item->next;
         }

      }

   }

}


void test()
{
    table TS;
    tsInit(&TS);
    int result;

    tClass *class = calloc(1, sizeof(tClass));
    class->name = "ssss";
    tsInsert(&TS, class->name, class);
    

    printf("%d\n",sizeof(tClass) );


   tFunc *func = calloc(1, sizeof(tFunc));
   if(!func) { return 1; }
   func->name = "substr";
   func->retType = TYPE_STRING;

   tFuncParam *ptr;
   tFuncParam *param = calloc(1, sizeof(tFuncParam));
   if(!param) { return 1; }

   ptr = param;
   func->paramCnt++;
   func->param = param;
   func->param->id = "s";
   func->param->type = TYPE_STRING;

   param = calloc(1, sizeof(tFuncParam));
   if(!param) { return 1; }

   ptr->nextParam = param;
   ptr = param;

   func->paramCnt++;
   param->id = "i";
   param->type = TYPE_INT;

   param = calloc(1, sizeof(tFuncParam));
   if(!param) { return 1; }

   ptr->nextParam = param;
   ptr = param;

   func->paramCnt++;
   param->id = "n";
   param->type = TYPE_INT;

   result = tsInsert (class->data, func->name, func);
   if(result == 1) return 1;
   else if(result == 2) { debug("%s %s\n", "Redifined func", ""); return 1; }
   else debug("%s %s %s\n", "func", func->name, "added to ST");

   tList *data = tsRead(class->data, func->name);
   if(data)
      data->func = true;
    //  printFunction(func);


   tVar *var = calloc(1, sizeof(tVar));
         if(!var) { return 1; }
         var->id = "promena";
         var->type = TYPE_STRING;
         var->init = true;

         result = tsInsert (class->data, var->id,var);
         if(result == 1) return 1;
         else if(result == 2) { debug("%s %s\n", "Redifined var", ""); return 1; }
         else debug("%s %s %s\n", "variable", var->id, "added to ST");



        tList *dataa = tsRead(class->data, var->id);
            if(dataa)
               dataa->func = false;

   printTSa(&TS);
}

































/*
hTable *htableInit( unsigned long size ) {

    hTable *table = malloc( sizeof(hTable) + size * sizeof(table->data[0]) );
    if(!table)
        return NULL;

    table->size          = size;
    table->hashFunction = &hashFunction;
    table->n             = 0;

    for( unsigned int i = 0; i < size; i++ )
        table->data[i] = NULL;

    return table;

}

unsigned int hashFunction(const char *str, unsigned htab_size)
{
    unsigned int h=0;
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % htab_size;
}

tToken * htableLookupAdd(hTable *table, const char *key)
{
    if( !table || !key )
        return NULL;

    unsigned int index = table->hashFunction(key, table->size);

    tToken* item = table->data[index];

    while(item)
    {
        if( !strcmp(item->key,key) ) 
            return item;

        else if( item->next ) 
            item = item->next;

        else 
            break;
    }


    tToken* tmp = malloc( sizeof(tToken) );
    if(!tmp)
        return NULL;

    tmp->key = malloc( sizeof( const char ) * ( strlen(key) + 1 ) );
    if(!tmp->key)
    {
        free(tmp);
        return NULL;
    }

    tmp->next = NULL;

    strcpy(tmp->key, key);

    if(item)
        item->next = tmp;

    else
        table->data[index] = tmp;

    return tmp;
}*/
