#include "ifj16Func.h"
#include "ial.h"
#include "ifj.h"
#include "debug.h"

int addFunction(table *TS, tFunc *func)
{
   int result;
   if (tsInsert (TS, func->name, func)) { return INTERNAL_ERROR; }
   tList *data = tsRead(TS, func->name);
   if (data) data->func = true;
   else return INTERNAL_ERROR;

   return SUCCESS;
}

int addFunctionParam(tFunc *func, tType typ, char *id)
{
   tFuncParam *param = calloc(1, sizeof(tFuncParam));
   if(NULL == param) { return INTERNAL_ERROR; }

   func->paramCnt++;
   param->id = id;
   param->type = typ;

   if(func->param)
   {
      tFuncParam *curr = func->param;
      while(curr->nextParam)
      {
         curr = curr->nextParam;
      }

      curr->nextParam = param;
   }
   else func->param = param;

   return SUCCESS;
}

int addIFJ16(table *TS)
{
   int result;

   tClass *IFJ16 = calloc(1, sizeof(tClass));
   if (!IFJ16) { return INTERNAL_ERROR; }

   IFJ16->isDeclared = true;
   if (tsInsert (TS, "ifj16", IFJ16)) { return INTERNAL_ERROR; }
   debugClass("%s\n", "Class ifj16 pridana to TS");

   tList *data = tsRead(TS, "ifj16");
   if(!data) { return INTERNAL_ERROR; }

   tFunc *func;
   if (NULL == (func = calloc(1, sizeof(tFunc)))) { return INTERNAL_ERROR; }
   func->name = "readInt";
   func->retType = TYPE_INT;
   func->isDeclared = true;
   if (addFunction(IFJ16->symbolTable, func)) { return INTERNAL_ERROR; }

   if (NULL == (func = calloc(1, sizeof(tFunc)))) { return INTERNAL_ERROR; }
   func->name = "readDouble";
   func->retType = TYPE_DOUBLE;
   func->isDeclared = true;
   if (addFunction(IFJ16->symbolTable, func)) { return INTERNAL_ERROR; }

   if (NULL == (func = calloc(1, sizeof(tFunc)))) { return INTERNAL_ERROR; }
   func->name = "readString";
   func->retType = TYPE_STRING;
   func->isDeclared = true;
   if (addFunction(IFJ16->symbolTable, func)) { return INTERNAL_ERROR; }

   // TODO -- PARAMTERY
   if (NULL == (func = calloc(1, sizeof(tFunc)))) { return INTERNAL_ERROR; }
   func->name = "print";
   func->retType = TYPE_VOID;
   func->isDeclared = true;
   if (addFunction(IFJ16->symbolTable, func)) { return INTERNAL_ERROR; }

   if (NULL == (func = calloc(1, sizeof(tFunc)))) { return INTERNAL_ERROR; }
   func->name = "length";
   func->retType = TYPE_INT;
   func->isDeclared = true;
   if (addFunctionParam(func, TYPE_STRING, "s")) { return INTERNAL_ERROR; }
   if (addFunction(IFJ16->symbolTable, func)) { return INTERNAL_ERROR; }

   if (NULL == (func = calloc(1, sizeof(tFunc)))) { return INTERNAL_ERROR; }
   func->name = "substr";
   func->retType = TYPE_INT;
   func->isDeclared = true;
   if (addFunctionParam(func, TYPE_STRING, "s")) { return INTERNAL_ERROR; }
   if (addFunctionParam(func, TYPE_INT, "i")) { return INTERNAL_ERROR; }
   if (addFunctionParam(func, TYPE_INT, "n")) { return INTERNAL_ERROR; }
   if (addFunction(IFJ16->symbolTable, func)) { return INTERNAL_ERROR; }

   if (NULL == (func = calloc(1, sizeof(tFunc)))) { return INTERNAL_ERROR; }
   func->name = "compare";
   func->retType = TYPE_INT;
   func->isDeclared = true;
   if (addFunctionParam(func, TYPE_STRING, "s1")) { return INTERNAL_ERROR; }
   if (addFunctionParam(func, TYPE_STRING, "s2")) { return INTERNAL_ERROR; }
   if (addFunction(IFJ16->symbolTable, func)) { return INTERNAL_ERROR; }

   if (NULL == (func = calloc(1, sizeof(tFunc)))) { return INTERNAL_ERROR; }
   func->name = "find";
   func->retType = TYPE_INT;
   func->isDeclared = true;
   if (addFunctionParam(func, TYPE_STRING, "s")) { return INTERNAL_ERROR; }
   if (addFunctionParam(func, TYPE_STRING, "search")) { return INTERNAL_ERROR; }
   if (addFunction(IFJ16->symbolTable, func)) { return INTERNAL_ERROR; }

   if (NULL == (func = calloc(1, sizeof(tFunc)))) { return INTERNAL_ERROR; }
   func->name = "sort";
   func->retType = TYPE_STRING;
   func->isDeclared = true;
   if (addFunctionParam(func, TYPE_STRING, "s")) { return INTERNAL_ERROR; }
   if (addFunction(IFJ16->symbolTable, func)) { return INTERNAL_ERROR; }

   return SUCCESS;
}





