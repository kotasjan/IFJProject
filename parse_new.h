#pragma once
#include "ial.h"

typedef struct stack
{
   table *table;
   struct stack *next;
} tStack;


int parse();   
int tsInsertClass(tClass **class);
int prog();
int prvkyTridy(table *TS, tStack *stack);

tStack *createStack(tStack *stack, table *TS);
int createVar(tVar **var, char *id, tType typ, bool init);
int type(tType *typ);
int createFunc(tFunc **func, char *id, tType typ);
int addFuncParam(tFunc *func, tType typ);