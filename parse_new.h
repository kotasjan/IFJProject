#pragma once
#include "ial.h"
#include "scaner.h"
#include <stdlib.h>



extern table *globalTS;

int parse();   
int tsInsertClass(tClass **class);
int prog();
int prvkyTridy(table *TS);

tStack *createStack(tStack *stack, table *TS);
int createVar(tVar **var, char *id, tType typ, bool init);
int type(tType *typ);
int createFunc(tFunc **func, char *id, tType typ);
int addFuncParam(tFunc *func, tType typ);
tList *lookVarStack(tFunc *func);
tList *tsCheck(table *TS);

struct toka
{
   tToken tok;
   struct toka *next;
};