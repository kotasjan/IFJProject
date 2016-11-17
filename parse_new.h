#pragma once
#include "ial.h"
#include "scaner.h"
#include <stdlib.h>

typedef struct
{
   tToken token[2];
   unsigned top;

} tTokenStack;

typedef struct stack
{
   table *table;
   struct stack *next;
} tStack;

extern tTokenStack tokenStack;
extern table *globalTS;

int parse();   
int tsInsertClass(tClass **class);
int prog();
int prvkyTridy(table *TS, tStack *stack);

tStack *createStack(tStack *stack, table *TS);
int createVar(tVar **var, char *id, tType typ, bool init);
int type(tType *typ);
int createFunc(tFunc **func, char *id, tType typ);
int addFuncParam(tFunc *func, tType typ);