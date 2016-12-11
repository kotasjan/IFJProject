#include "parse_new.h"
#include "ifj.h"
#include "ial.h"
#include "scaner.h"
#include "debug.h"
#include "ifj16func.h"
#include "expr.h"
#include "tokenfifo.h"
#include "instruction.h"
#include <string.h>

tToken token;
table *globalTS = NULL;

tInstructionList *blokList;
tInstructionList *globalList = NULL;

tVar **tokenParam = NULL;

int parse()
{
   int result;
   table TS;
   tsInit(&TS);
   globalTS = &TS;


   if ((result = addIFJ16(&TS))) { return result; }

   if ((result = getToken(&token))) { debug("%s\n", "Chyba v prvnim tokenu"); return result; }
   if ((result = GlobalWalkthrought())) { return result; }

   if (controlMainRun()) { return SEM_ERROR; }

   //printTS(globalTS);

   setFileToBegin(); // nastavi soubor na zacatek, 2. pruchod

   if (!(globalList = listInit())) return INTERNAL_ERROR;

   getToken(&token);
   if ((result = prog(&TS))) { return result; }
   ////printf("ok\n");

  // if ((result = semStart())) { //printf("rtt"); return result; }
   tList *data = tsRead(globalTS, "Main");
   data = tsRead(((tClass*)(data->dataPtr))->symbolTable, "run");
   ActualList = globalList;
   listInsert(generateInstruction(I_SET_START,NULL,NULL,data->dataPtr));
   interpret(globalList);
   return SUCCESS;
}

tClass *getClassTS()
{
   int result;
   tList *data = tsRead(globalTS, token.id);
   tClass *class = data->dataPtr;
   return class;
}

tFunc *getFunc(table *TS, char *id)
{
   int result;
   tList *data = tsRead(TS, id);
   if (!data) return NULL;
   if (!data->func) return NULL;
   tFunc *func = data->dataPtr;
   return func;
}

// VSE OK - HOTOVO
int prog()
{
   int result;

   switch (token.type)
   {
      // EOF
      case END_OF_FILE: { return SUCCESS; }

      case CLASS:
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } // ID
         if (token.type != IDENTIFIER) { debug("%s\n", "ERROR: <class ID>"); return SYNTAX_ERROR; }
         tClass *class = getClassTS();
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } // {
         if (token.type != LEFT_CURLY_BRACKET) { debug("chyby { -- %s\n", printTok(&token)); return SYNTAX_ERROR; } 
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } // prvky tridy, }
         if ((result = prvkyTridy(class->symbolTable))) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("chyby } -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = prog())) { return result; }
         return SUCCESS;
      
      default:
         break;
   }

   return SYNTAX_ERROR;
}

// VSE OK - HOTOVO
int prvkyTridy(table *TS)
{
   int result;

   switch (token.type)
   {
      case STATIC:
      {
         ActualList = globalList;
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = zavStrRov(TS))) { return result; }
         if ((result = prvkyTridy(TS))) { return result; }
         return result;
      }
      case RIGHT_CURLY_BRACKET:
         return SUCCESS;
      default:
         break;
   }
   return SYNTAX_ERROR;
}


char *parseFullId(char **class, char *name)
{
   char *id = strtok(name, ".");
   bool inClass = true;
   while (id)
   {
      if(inClass) { *class = id; inClass = false; }
      else return id;

      id = strtok(NULL, ".");
   }

   return NULL;
}

int zavStrRov(table *TS)
{
   int result;

   switch (token.type)
   {
      case INT:
      case DOUBLE:
      case STRING:
      case VOID:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } // ID
         if (token.type != IDENTIFIER) { return SYNTAX_ERROR; }
         char *id = token.id;
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 

         switch (token.type)
         {
            case SEMICOLON:
            {
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
               return result;
            }
            case LEFT_BRACKET:
            {
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
               if ((result = parametr(NULL))) { return result; }
               if (token.type != RIGHT_BRACKET) { return SYNTAX_ERROR; }
               tFunc *func = getFunc(TS, id);
               if (!func) { return SYNTAX_ERROR; }
               tInstructionList *act =ActualList;
               func->List = listInit();
               ActualList = func->List;
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
               if (token.type != LEFT_CURLY_BRACKET) { return SYNTAX_ERROR; }
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
               if ((result = veFunkci(func))) { return result; }

               if (token.type != RIGHT_CURLY_BRACKET) { return SYNTAX_ERROR; }
               ActualList = act;
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
               return result;
            }
            case ASSIGNMENT:
            {
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
               tToken tmp;
               memcpy(&tmp, &token, sizeof(tToken));
               tStack stack;
               stack.next = NULL;
               stack.table = TS;
               if (token.type == IDENTIFIER || token.type == FULL_IDENTIFIER)
               {
                  char *funcName = token.id;
                  if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                  if (token.type == LEFT_BRACKET)
                  {
                     return SEM_OTHER_ERROR;
                  }
                  else
                  {
                     if (insertTokenFifo(&tmp)) { return INTERNAL_ERROR; }
                     if (insertTokenFifo(&token)) { return INTERNAL_ERROR; }
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                     if ((result = checkExpr())) { return result; }
                     if ((result = expression(false, &stack))) {  return result; } 
                     if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                     tList *data = tsRead(TS, id);
                     if (!data) return SEM_ERROR;
                     if (data->func) return SEM_ERROR;
                     tVar *var = data->dataPtr;

                     listInsert(generateInstruction(I_ASSIGNMENT,vysExp,NULL,var));
                     
                  }
               }
               else
               {
                  if ((result = checkExpr())) { return result; }
                  if ((result = expression(false, &stack))) { return result; } 
                  if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                  if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                  tList *data = tsRead(TS, id);
                  if (!data) return SEM_ERROR;
                  if (data->func) return SEM_ERROR;
                  tVar *var = data->dataPtr;
                  listInsert(generateInstruction(I_ASSIGNMENT,vysExp,NULL,var));
               }
               return result;

            }
         }
      }
      default:
         break;
   }

   return SYNTAX_ERROR;
}

tList *lookVarStack(tFunc *func)
{
   tList *data = tsRead(func->stack->table, token.id);
   if (data) { debug("nasel sem parametr '%s' funcke '%s'\n", token.id, func->name);  return data; }
   data = tsRead(func->stack->next->table, token.id);
   if (data) { debug("nasel sem parametr '%s' funcke '%s'\n", token.id, func->name);  return data; }
   debug("Promenna '%s' neni deklarovana\n", token.id);
   return NULL;
}

int checkExpr()
{
   switch (token.type)
   {
      case IDENTIFIER:
      case FULL_IDENTIFIER:
      case LIT_INT:
      case LIT_DOUBLE:
      case CHAIN:
      case LEFT_BRACKET:
         return SUCCESS;
      default:
         break;
   }
   return SYNTAX_ERROR;
}


int rovnFun(tFunc *func)
{
   int result;

char *className = NULL;
   switch (token.type)
   {
      
      case IDENTIFIER:
      case FULL_IDENTIFIER:
      {
         tList *data;
         char *id = token.id;
         tClass *class;
         if (token.type == IDENTIFIER)
         {
            data = lookVarStack(func);
            if (!data) {  return SEM_ERROR; }
            class = data->dataPtr;

            // data = tsCheck(func->stack->table);
            if (!data) { debug("Nedeklarovana funkce/promenna '%s'\n", token.id); return SEM_ERROR; }
            if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 

         }
         else
         {
            id = parseFullId(&className, token.id);
            if (!id) { return SYNTAX_ERROR; }
            data = tsRead(globalTS, className);
            if (!data) { debug("Nedeklarovana funkce/promenna\n"); return SEM_ERROR; }
            class = data->dataPtr;
            data = tsRead(class->symbolTable, id);
            if (!data) { debug("Nedeklarovana funkce/promenna\n"); return SEM_ERROR; }
            if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 

         }

         switch (token.type)
         {
            case ASSIGNMENT:
            {
               if (data->func) { debug("Promenna je funkce\n"); return SEM_ERROR; }
               tVar *var = data->dataPtr;

               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
               tToken tmp;
               memcpy(&tmp, &token, sizeof(tToken));
               if (token.type == IDENTIFIER || token.type == FULL_IDENTIFIER)
               {
                  char *funcName = token.id;
                  if (token.type == IDENTIFIER) //  a = c();
                  {
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                     if (token.type == LEFT_BRACKET)
                     {
                        if (tsCheckFunc(func->stack->next->table, funcName)) {  return SYNTAX_ERROR; }
                        tFunc *callFunc;
                        if (createFunc(&callFunc, funcName, var->type)) { return INTERNAL_ERROR; }
                        tList *data = tsRead(func->stack->next->table, funcName);
                        if (!data) { return SEM_ERROR; }
                        tFunc *funcCmp = data->dataPtr;
                        if (!(tokenParam = calloc(funcCmp->paramCnt, sizeof(tVar*)))) return INTERNAL_ERROR;

                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        if ((result = parametrVolani(func, callFunc))) { return result; }
                        if (token.type != RIGHT_BRACKET) { return SYNTAX_ERROR; }
                        
                        
                        if (compareFunction(funcCmp, callFunc)) { return SEM_TYPE_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        listInsert(generateInstruction(I_CALL,funcCmp,tokenParam,var));
                        listInsert(generateInstruction(I_SET_RET,funcCmp,tokenParam,NULL));
                        listInsert(generateInstruction(I_FUNC_CMP,funcCmp,NULL,NULL));
                        return result;
                     }
                     else 
                     {
                        setRetType(var->type);
                        if (var->type == TYPE_STRING) listInsert(generateInstruction(I_SET_CONCAT,NULL,NULL,NULL));
                        if (insertTokenFifo(&tmp)) { return INTERNAL_ERROR; }
                        if (insertTokenFifo(&token)) { return INTERNAL_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        if (checkExpr()) { return SYNTAX_ERROR; }
                        if ((result = expression(false, func->stack))) { return result; } 
                        if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        listInsert(generateInstruction(I_ASSIGNMENT,vysExp,NULL,var));
                        return result;
                     }
                  }
                  else //  a = Main.c();
                  {
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                     if (token.type == LEFT_BRACKET)
                     {
                        char *className;
                        char *functionName;

                        functionName = parseFullId(&className, funcName);
                        if (!functionName) { return SYNTAX_ERROR; }
                        tList *data = tsRead(globalTS, className);
                        if (!data) { debug("Nedeklarovana funkce\n"); return SEM_ERROR; }
                        tClass *class = data->dataPtr;
                        data = tsRead(class->symbolTable, functionName);
                        if (!data) { debug("Nedeklarovana funkce\n"); return SEM_ERROR; }
                        if (!data->func) { debug("funkce neni funkce\n"); return SEM_ERROR;}
                        tFunc *callFunc;
                        if (createFunc(&callFunc, functionName, var->type)) { return INTERNAL_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        
                        tFunc *funcCmp = data->dataPtr;
                        if (!(tokenParam = calloc(funcCmp->paramCnt, sizeof(tVar*)))) return INTERNAL_ERROR;

                        if ((result = parametrVolani(func, callFunc))) { return result; }
                        if (token.type != RIGHT_BRACKET) { return SYNTAX_ERROR; }
                        
                        callFunc->className = className;
                        listInsert(generateInstruction(I_SET_CLASS,className,NULL,NULL));
                        if (compareFunction(funcCmp, callFunc)) { return SEM_TYPE_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        listInsert(generateInstruction(I_CALL,funcCmp,tokenParam,var));
                        if ((strcmp(className, "ifj16")))
                        {
                           listInsert(generateInstruction(I_SET_RET,funcCmp,tokenParam,NULL));
                           listInsert(generateInstruction(I_FUNC_CMP,funcCmp,NULL,NULL));
                        }  
          
                        return result;
                     }
                     else
                     {
                        setRetType(var->type);
                        if (var->type == TYPE_STRING) listInsert(generateInstruction(I_SET_CONCAT,NULL,NULL,NULL));
                        if (insertTokenFifo(&tmp)) { return INTERNAL_ERROR; }
                        if (insertTokenFifo(&token)) { return INTERNAL_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        if (checkExpr()) { return SYNTAX_ERROR; }
                        if ((result = expression(false, func->stack))) { return result; } 
                        if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        listInsert(generateInstruction(I_ASSIGNMENT,vysExp,NULL,var));
                        return result;
                     }
                  }
               }
               else
               {
                  setRetType(var->type);
                  if (var->type == TYPE_STRING) listInsert(generateInstruction(I_SET_CONCAT,NULL,NULL,NULL));
                  if (checkExpr()) { return SYNTAX_ERROR; }
                  if ((result = expression(false, func->stack))) { return result; } 
                  if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                  if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                  listInsert(generateInstruction(I_ASSIGNMENT,vysExp,NULL,var));
                  return result;
               }
               
            }
            case LEFT_BRACKET:
            {
               tVar *var = calloc(1, sizeof(tVar));
               if (!var) return INTERNAL_ERROR;
               var->type = TYPE_VOID;
               if (className && (!(strcmp(className, "ifj16"))))
               {
                     if((result = proccessPrint(func))) {return result;}
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
               
               }
               else
               {
                  //printf("dddd\n");

                  if(className){
                  
                     tList *data = tsRead(globalTS, className);
                     if (!data) { return SEM_ERROR; }
                     if (tsCheckFunc(((tClass*)(data->dataPtr))->symbolTable, id)) { return SEM_ERROR; }
                     data = tsRead(((tClass*)(data->dataPtr))->symbolTable, id);
                     if (!data) { return SEM_ERROR; }
                     tFunc *funcCmp = data->dataPtr;
                     tFunc *callFunc;
                     if (createFunc(&callFunc, id, TYPE_VOID)) { return INTERNAL_ERROR; }
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                     if (!(tokenParam = calloc(funcCmp->paramCnt, sizeof(tVar*)))) return INTERNAL_ERROR;
                     if ((result = parametrVolani(func, callFunc))) { return result; }
                     if (token.type != RIGHT_BRACKET) { return SYNTAX_ERROR; }
                     listInsert(generateInstruction(I_SET_CLASS,className,NULL,NULL));

                     callFunc->className = className;
                     if (compareFunction(funcCmp, callFunc)) { return SEM_TYPE_ERROR; }
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                     if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; } 
                     listInsert(generateInstruction(I_CALL,funcCmp,tokenParam,var));
                     if ((strcmp(className, "ifj16")))
                     {
                        listInsert(generateInstruction(I_SET_RET,funcCmp,tokenParam,NULL));
                        listInsert(generateInstruction(I_FUNC_CMP,funcCmp,NULL,NULL));
                     } 

/*
                     if(callFunc->retType == TYPE_VOID)
                        listInsert(generateInstruction(I_SET_RET,funcCmp,tokenParam,var));
                     else
                        listInsert(generateInstruction(I_SET_RET,funcCmp,tokenParam,var));
                     listInsert(generateInstruction(I_FUNC_CMP,funcCmp,NULL,NULL));
                     */
                  }
                  else
                  {
                     //printf("asdasdsa\n");
                     if (tsCheckFunc(func->stack->next->table, id)) {  return SEM_ERROR; }
                     tList *data = tsRead(func->stack->next->table, id);
                     if (!data) { return SEM_ERROR; }
                     tFunc *funcCmp = data->dataPtr;
                     tFunc *callFunc;
                     if (createFunc(&callFunc, id, TYPE_VOID)) { return INTERNAL_ERROR; }
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                     if (!(tokenParam = calloc(funcCmp->paramCnt, sizeof(tVar*)))) return INTERNAL_ERROR;
                     if ((result = parametrVolani(func, callFunc))) { return result; }
                     
                     if (token.type != RIGHT_BRACKET) { return SYNTAX_ERROR; }
                     
                     
                     callFunc->className = className;
                     if (compareFunction(funcCmp, callFunc)) { return SEM_TYPE_ERROR; }
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                     if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; } 
                     listInsert(generateInstruction(I_CALL,funcCmp,tokenParam,var));
                     if(var->type == TYPE_VOID)
                        listInsert(generateInstruction(I_SET_RET,funcCmp,tokenParam,var));
                     else
                        listInsert(generateInstruction(I_SET_RET,funcCmp,tokenParam,NULL));
                        listInsert(generateInstruction(I_FUNC_CMP,funcCmp,NULL,NULL));
                     
                     /*
                     listInsert(generateInstruction(I_SET_RET,funcCmp,tokenParam,var));
                     listInsert(generateInstruction(I_FUNC_CMP,funcCmp,NULL,NULL));
                     */
                  }
               }
               return result;
            }
         }
         return result;
      }
      default:
         break;
   }

   return SYNTAX_ERROR;
}

int proccessPrint(tFunc *func)
{

   int result;
   if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
   tVar *var = calloc(1, sizeof(tVar));
   if (!var) return INTERNAL_ERROR;

   switch(token.type)
   {
      case LIT_INT:
         var->type = TYPE_INT;
         var->init = true;
         var->value.intValue = atoi(token.id);
         listInsert(generateInstruction(I_PRINT,NULL,NULL,var));   
         if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
         if (token.type == RIGHT_BRACKET) return SUCCESS;
         else if (token.type != PLUS) return SYNTAX_ERROR;
         if ((result = proccessPrint(func))) { return result; }
         return SUCCESS; 

      case LIT_DOUBLE:
         var->type = TYPE_DOUBLE;
         var->init = true;
         char *ptr;
         var->value.doubleValue = strtod(token.id, &ptr);
         listInsert(generateInstruction(I_PRINT,NULL,NULL,var));   
         if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
         if (token.type == RIGHT_BRACKET) return SUCCESS;
         else if (token.type != PLUS) return SYNTAX_ERROR;
         if ((result = proccessPrint(func))) { return result; }
         return SUCCESS;
      case CHAIN:
         var->type = TYPE_STRING;
         var->init = true;
         var->value.stringValue = token.id;
         listInsert(generateInstruction(I_PRINT,NULL,NULL,var));   
         if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
         if (token.type == RIGHT_BRACKET) return SUCCESS;
         else if (token.type != PLUS) return SYNTAX_ERROR;
         if ((result = proccessPrint(func))) { return result; }
         return SUCCESS;
      case IDENTIFIER:
      {
         tList *data = tsRead(func->stack->table, token.id);
         if (!data)
         {
            data = tsRead(func->stack->next->table, token.id);
            if (!data)
            {
               debug("Undef reference\n");
               return SEM_ERROR;
            }
         }

         var = data->dataPtr;
         listInsert(generateInstruction(I_PRINT,NULL,NULL,var));   
         if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
         if (token.type == RIGHT_BRACKET) return SUCCESS;
         else if (token.type != PLUS) return SYNTAX_ERROR;
         if ((result = proccessPrint(func))) { return result; }
         return SUCCESS;     
      }
      case FULL_IDENTIFIER:
      {
         char *className;
         char *functionName;

         functionName = parseFullId(&className, functionName);
         if (!functionName) { return SYNTAX_ERROR; }
         tList *data = tsRead(globalTS, className);
         if (!data) { debug("Nedeklarovana promenna\n"); return SEM_ERROR; }
         tClass *class = data->dataPtr;
         data = tsRead(class->symbolTable, functionName);
         if (!data) { debug("Nedeklarovana promenna\n"); return SEM_ERROR; }
         if (data->func) { debug("naopak funkce neni promenna\n"); return SEM_ERROR;}
         var = data->dataPtr;
         listInsert(generateInstruction(I_PRINT,NULL,NULL,var));   
         if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
         if (token.type == RIGHT_BRACKET) return SUCCESS;
         else if (token.type != PLUS) return SYNTAX_ERROR;
         if ((result = proccessPrint(func))) { return result; }
         return SUCCESS;     

      }
      case RIGHT_BRACKET:
      return SUCCESS;

      default:
      return SYNTAX_ERROR;

   }

}

int controlDecTable(table *TS, char *key)
{
   tList *data = tsRead(TS, key);
   if (data) { debug("Redeklarovana promena/ funkce %s\n", key); return SEM_ERROR; }
      
   return SUCCESS;
}


int strRovn(tFunc *func)
{
   int result;

   switch (token.type)
   {
      case INT:
      case DOUBLE:
      case STRING:
      {
         tVar *var;
         if (NULL == (var = calloc(1, sizeof(tVar)))) { return INTERNAL_ERROR; }
         var->type = token.type;
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != IDENTIFIER) { debug("Promenna nema identifkator\n"); return SYNTAX_ERROR; }
         var->id = token.id;
         if (tsCheckVarFunc(func->stack->table, var->id)) { return SEM_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = tsInsertVar(func->stack->table, var))) { return result; }
         debug("pridal jsem promennou '%s' do funkce '%s'\n", var->id, func->name);
         
         switch (token.type)
         {
            case SEMICOLON:
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
               return result;

            case ASSIGNMENT:
            {
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
               tToken tmp;
               memcpy(&tmp, &token, sizeof(tToken));
               if (token.type == IDENTIFIER || token.type == FULL_IDENTIFIER)
               {
                  char *funcName = token.id;
                  if (token.type == IDENTIFIER) // int a = c();
                  {
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                     if (token.type == LEFT_BRACKET)
                     {
                        if (tsCheckFunc(func->stack->next->table, funcName)) {  return SEM_ERROR; }
                        tFunc *callFunc;
                        if (createFunc(&callFunc, funcName, var->type)) { return INTERNAL_ERROR; }
                        tList *data = tsRead(func->stack->next->table, funcName);
                        if (!data) { return SEM_ERROR; }
                        tFunc *funcCmp = data->dataPtr;
                        if (!(tokenParam = calloc(funcCmp->paramCnt, sizeof(tVar*)))) return INTERNAL_ERROR;
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        if ((result = parametrVolani(func, callFunc))) { return result; }
                        if (token.type != RIGHT_BRACKET) { return SYNTAX_ERROR; }

                        if (compareFunction(funcCmp, callFunc)) { return SEM_TYPE_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        listInsert(generateInstruction(I_CALL,funcCmp,tokenParam,var));

                        listInsert(generateInstruction(I_SET_RET,funcCmp,tokenParam,NULL));
                        listInsert(generateInstruction(I_FUNC_CMP,funcCmp,NULL,NULL));
                        return result;
                     }
                     else 
                     {
                        setRetType(var->type);
                        if (var->type == TYPE_STRING) listInsert(generateInstruction(I_SET_CONCAT,NULL,NULL,NULL));
                        if (insertTokenFifo(&tmp)) { return INTERNAL_ERROR; }
                        if (insertTokenFifo(&token)) { return INTERNAL_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        if (checkExpr()) { return SYNTAX_ERROR; }
                        if ((result = expression(false, func->stack))) { return result; } 
                        if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        listInsert(generateInstruction(I_ASSIGNMENT,vysExp,NULL,var));
                        
                        return result;
                     }
                  }
                  else // int a = Main.c();
                  {
                     if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                     if (token.type == LEFT_BRACKET)
                     {
                        char *className;
                        char *functionName;

                        functionName = parseFullId(&className, funcName);
                        if (!functionName) { return SYNTAX_ERROR; }
                        tList *data = tsRead(globalTS, className);
                        if (!data) { debug("Nedeklarovana funkce\n"); return SEM_ERROR; }
                        tClass *class = data->dataPtr;
                        data = tsRead(class->symbolTable, functionName);
                        if (!data) { debug("Nedeklarovana funkce\n"); return SEM_ERROR; }
                        if (!data->func) { debug("funkce neni funkce\n"); return SEM_ERROR;}
                        tFunc *callFunc;
                        if (createFunc(&callFunc, functionName, var->type)) { return INTERNAL_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        tFunc *funcCmp = data->dataPtr;
                        if (!(tokenParam = calloc(funcCmp->paramCnt, sizeof(tVar*)))) return INTERNAL_ERROR;
                        if ((result = parametrVolani(func, callFunc))) { return result; }
                        if (token.type != RIGHT_BRACKET) { return SYNTAX_ERROR; }
                        
                        callFunc->className = className;
                        listInsert(generateInstruction(I_SET_CLASS,className,NULL,NULL));
                        if (compareFunction(funcCmp, callFunc)) { return SEM_TYPE_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        listInsert(generateInstruction(I_CALL,funcCmp,tokenParam,var));
                        if ((strcmp(className, "ifj16")))
                        {
                           listInsert(generateInstruction(I_SET_RET,funcCmp,tokenParam,NULL));
                           listInsert(generateInstruction(I_FUNC_CMP,funcCmp,NULL,NULL));
                        } 
                        /*
                        listInsert(generateInstruction(I_SET_RET,funcCmp,tokenParam,NULL));
                        listInsert(generateInstruction(I_FUNC_CMP,funcCmp,NULL,NULL));
                        */

                        return result;
                     }
                     else
                     {
                        setRetType(var->type);
                        if (var->type == TYPE_STRING) listInsert(generateInstruction(I_SET_CONCAT,NULL,NULL,NULL));
                        if (insertTokenFifo(&tmp)) { return INTERNAL_ERROR; }
                        if (insertTokenFifo(&token)) { return INTERNAL_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        if (result = checkExpr()) { return result; }
                        if ((result = expression(false, func->stack))) { return result; } 
                        if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                        if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                        listInsert(generateInstruction(I_ASSIGNMENT,vysExp,NULL,var));
                        return result;
                     }
                  }
               }
               else
               {
                  setRetType(var->type);
                  if (var->type == TYPE_STRING) listInsert(generateInstruction(I_SET_CONCAT,NULL,NULL,NULL));
                  if ((result = checkExpr())) { return result; }
                  if ((result = expression(false, func->stack))) { return result; } 
                  if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
                  if ((result = getToken(&token))) { debug("expre - v LEX\n"); return result; }
                  listInsert(generateInstruction(I_ASSIGNMENT,vysExp,NULL,var));
 
                  return result;
               }
            }
            default:
               return SYNTAX_ERROR;
            }
            
         }

      case VOID:
      {
         return SEM_ERROR;
      }
      default:
         break;
   }

   return SYNTAX_ERROR;
}

table *tsCheckClass(char *className)
{
   int result;
   tList *data = tsRead(globalTS, className);
   tClass *class;
   
   // pokud jeste neni trida v tabulce symbolu z predchoziho volani, tak ji tam pridam
   if (NULL == data)
   {
      // inicializuji data
      if (NULL == (class = calloc(1, sizeof(tClass)))) { return NULL; }
      class->name = className;
      class->isDeclared = false;

      if ((result = tsInsert(globalTS, class->name, class)) == 1) { return NULL; }
      debugClass("Class %s pridana do TS\n", class->name);
      data = tsRead(globalTS, className);
   }

   class = data->dataPtr;

   return class->symbolTable;
} 



int tsCheckVarFunc(table *TS, char *id)
{
   tList *data = tsRead(TS, id);
   if (data)
   {
      debug("redeklarace promenne '%s'\n", id);
     return SEM_ERROR;
   }
   else
   {
      return SUCCESS;
   }
}

tList *tsCheck(table *TS)
{
   tList *data = tsRead(TS, token.id);
   if (data)
   {
      return data;
   }
   else
   {
      return NULL;
   }

}

int tsCheckFunc(table *TS, char *id)
{

   tList *data = tsRead(TS, id);
   if (data)
   {

      if(!data->func) return SEM_ERROR;
      return SUCCESS;
   }
   else
   {
      return SEM_ERROR;
   }
}

int boolExp()
{
   int result;

   switch (token.type)
   {
      default:
      {
         int bracket = 1;
         while(token.type != RIGHT_BRACKET)
         {
            if(token.type == LEFT_BRACKET) { bracket++; }
            if((result = getToken(&token))) { debug("%s\n", "ERROR - v LEX"); return result; }   
            if (token.type == END_OF_FILE) return SYNTAX_ERROR;
            if(token.type == RIGHT_BRACKET) {
               while(token.type == RIGHT_BRACKET)
               {
                  bracket--; 
                  if(bracket) if((result = getToken(&token))) { debug("%s\n", "ERROR - v LEX"); return result; } 
                  if (token.type == END_OF_FILE) return SYNTAX_ERROR;
                  if(!bracket) break;
               }  
            }    
         }
         return result;
      }
   }

   return SYNTAX_ERROR;

}

int blok(tFunc *func)
{
   int result;
   switch (token.type)
   {
      case IDENTIFIER:
      case FULL_IDENTIFIER:
      {
         // listInsert(generateInstruction(I_TEST,vysExp,NULL,NULL));
         if ((result = rovnFun(func))) { return result; } 
         if ((result = blok(func))) { return result; } 
         return result;
      }
      case IF:
      {

         setRetType(TYPE_DOUBLE);
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_BRACKET) { debug("IF nema ( -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = checkExpr())) { return result; }
         if (expression(true, func->stack)) {  return SYNTAX_ERROR; }
         tInstructionList *act = ActualList;
         tInstructionItem *ifJMP = listInsert(generateInstruction(I_IF,vysExp,NULL,NULL));
         if (token.type != RIGHT_BRACKET) { debug("IF nema ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_CURLY_BRACKET) { debug("IF nema { -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if (!(blokList = listInit())) return INTERNAL_ERROR;  // init blok
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok(func))) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("IF nema } -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         listInsert(generateInstruction(I_SKIP_TAPE,NULL,NULL,NULL));
         ifJMP->Instruction->addr2 = ActualList; // pridani bloku
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != ELSE) { debug("IF nema ELSE ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != LEFT_CURLY_BRACKET) { debug("IF nema { -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (!(blokList = listInit())) return INTERNAL_ERROR; // init blok
         if ((result = blok(func))) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("IF nema } -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         listInsert(generateInstruction(I_SKIP_TAPE,NULL,NULL,NULL));
         ifJMP->Instruction->addr3 = ActualList; // pridani bloku
         ActualList = act; // nastaveni zpet       
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = blok(func))) { return result; } 
         return result;
      }
      case DO:
      {
         setRetType(TYPE_DOUBLE);
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_CURLY_BRACKET) { debug("DO nema { -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok(func))) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("DO nema } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != WHILE) { debug("DO nema WHILE ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_BRACKET) { debug("DO nema ( -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = checkExpr())) { return result; }
         if (expression(true, func->stack)) { return SYNTAX_ERROR; }  
         if (token.type != RIGHT_BRACKET) { debug("DO nema ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != SEMICOLON) { debug("DO ; } -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = blok(func))) { return result; } 
         return result;  
      }
      case WHILE:
      {
         setRetType(TYPE_DOUBLE);
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_BRACKET) { debug("WHILE nema ( -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         tInstructionItem *skipWhileJMP = listInsert(generateInstruction(I_SKIP_NEXT_INST,NULL,NULL,NULL));
         if ((result = checkExpr())) { return result; }
         if (expression(true, func->stack)) { return SYNTAX_ERROR; }

         tInstructionList *act = ActualList;
         tInstructionItem *whileJMP = listInsert(generateInstruction(I_WHILE,vysExp,NULL,NULL)); 
         if (!(blokList = listInit())) return INTERNAL_ERROR; // init blok  

         if (token.type != RIGHT_BRACKET) { debug("WHILE nema ) -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != LEFT_CURLY_BRACKET) { debug("WHILE nema { -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 

         if ((result = blok(func))) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("WHILE nema } -- %s\n", printTok(&token)); return SYNTAX_ERROR; }

         listInsert(generateInstruction(I_SKIP_TAPE,NULL,NULL,NULL));
         whileJMP->Instruction->addr2 = ActualList; // pridani bloku
         ActualList = act; // nastaveni zpet 
         listInsert(generateInstruction(I_SKIP_BEFORE_WHILE,skipWhileJMP,NULL,NULL));
         
         skipWhileJMP = listInsert(generateInstruction(I_SKIP_NEXT_INST,NULL,NULL,NULL));
         whileJMP->Instruction->addr3 = skipWhileJMP;
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }        
         if ((result = blok(func))) { return result; } 
         return result; 
      }
      case RETURN:
      {/*
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = checkExpr())) { return result; }
         if (expression(true, func->stack)) { return SYNTAX_ERROR; }       
         if ((result = blok(func))) { return result; } 
         return result; 
*/
         //printf("return\n");
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type == SEMICOLON)
         {
            if (func->retType != TYPE_VOID) { debug("Prazdny return v ne Void funkci\n"); return SYNTAX_ERROR; } 
            tInstructionItem *ifJMP = listInsert(generateInstruction(I_RETURN,NULL,func->funcTable,NULL));
         }
         else
         {
            //printf("FUNC RET TYPEEEEE %d\n",func->retType );
            if(func->retType == TYPE_STRING)
            {
               vysExp->type = TYPE_STRING;
               setRetType(TYPE_STRING);
            }
            if ((result = checkExpr())) { return result; }
            if (expression(false, func->stack)) { return SYNTAX_ERROR; }  
            if(func->retType == TYPE_STRING)
            {
               //printf("asdasdas\n"); exit(44);
               vysExp->type = TYPE_STRING;
            }
            tInstructionItem *ifJMP = listInsert(generateInstruction(I_RETURN,vysExp,func->funcTable,NULL));   
         }
         //printf("retasdasdurn\n");
              
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok(func))) { return result; } 
         //printf("ret87878urn\n");

         return result; 
      }
      default:
         return SUCCESS;

   }
}

int veFunkci(tFunc *func)
{

   int result;
   switch (token.type)
   {
      case INT:
      case DOUBLE:
      case STRING:
      case VOID:
      {
         if ((result = strRovn(func))) { return result; }
         if ((result = veFunkci(func))) { return result; }
         return result;
      }
      case IDENTIFIER:
      case FULL_IDENTIFIER:
      {
         if ((result = rovnFun(func))) { return result; } 
         if ((result = veFunkci(func))) { return result; }
         return result;
      }    
      case IF:
      {
         setRetType(TYPE_DOUBLE);
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_BRACKET) { debug("IF nema ( -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = checkExpr())) { return result; }
         if (expression(true, func->stack)) {  return SYNTAX_ERROR; }
         tInstructionList *act = ActualList;
         tInstructionItem *ifJMP = listInsert(generateInstruction(I_IF,vysExp,NULL,NULL));
         if (token.type != RIGHT_BRACKET) { debug("IF nema ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_CURLY_BRACKET) { debug("IF nema { -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if (!(blokList = listInit())) return INTERNAL_ERROR;  // init blok
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok(func))) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("IF nema } -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         listInsert(generateInstruction(I_SKIP_TAPE,NULL,NULL,NULL));
         ifJMP->Instruction->addr2 = ActualList; // pridani bloku
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != ELSE) { debug("IF nema ELSE ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != LEFT_CURLY_BRACKET) { debug("IF nema { -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (!(blokList = listInit())) return INTERNAL_ERROR; // init blok
         if ((result = blok(func))) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("IF nema } -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         listInsert(generateInstruction(I_SKIP_TAPE,NULL,NULL,NULL));
         ifJMP->Instruction->addr3 = ActualList; // pridani bloku
         ActualList = act; // nastaveni zpet       
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = veFunkci(func))) { return result; } 
         return result;
      }
      case DO:
      {
         setRetType(TYPE_DOUBLE);
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_CURLY_BRACKET) { debug("DO nema { -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = checkExpr())) { return result; }
         if (expression(true, func->stack)) { return SYNTAX_ERROR; } 
         if (token.type != RIGHT_CURLY_BRACKET) { debug("DO nema } -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != WHILE) { debug("DO nema WHILE ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_BRACKET) { debug("DO nema ( -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok(func))) { return result; }
         if (token.type != RIGHT_BRACKET) { debug("DO nema ) -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != SEMICOLON) { debug("DO ; } -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = veFunkci(func))) { return result; } 
         return result;  
      }
      case WHILE:
      {
         setRetType(TYPE_DOUBLE);
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_BRACKET) { debug("WHILE nema ( -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         tInstructionItem *skipWhileJMP = listInsert(generateInstruction(I_SKIP_NEXT_INST,NULL,NULL,NULL));
         if ((result = checkExpr())) { return result; }
         if (expression(true, func->stack)) { return SYNTAX_ERROR; }
         tInstructionList *act = ActualList;
         tInstructionItem *whileJMP = listInsert(generateInstruction(I_WHILE,vysExp,NULL,NULL)); 
         if (!(blokList = listInit())) return INTERNAL_ERROR; // init blok  
         if (token.type != RIGHT_BRACKET) { debug("WHILE nema ) -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != LEFT_CURLY_BRACKET) { debug("WHILE nema { -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 

         if ((result = blok(func))) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("WHILE nema } -- %s\n", printTok(&token)); return SYNTAX_ERROR; }

         whileJMP->Instruction->addr2 = ActualList; // pridani bloku
         listInsert(generateInstruction(I_SKIP_TAPE,NULL,NULL,NULL));
         ActualList = act; // nastaveni zpet 
         listInsert(generateInstruction(I_SKIP_BEFORE_WHILE,skipWhileJMP,NULL,NULL));
         
         skipWhileJMP = listInsert(generateInstruction(I_SKIP_NEXT_INST,NULL,NULL,NULL));
         whileJMP->Instruction->addr3 = skipWhileJMP;
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }        
         if ((result = veFunkci(func))) { return result; } 
         return result; 
      }
      case RETURN:
      {
         //printf("return\n");
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type == SEMICOLON)
         {
            //printf("sem tady\n\n\n\n\n");
            if (func->retType != TYPE_VOID) { debug("Prazdny return v ne Void funkci\n"); return SYNTAX_ERROR; } 
            tInstructionItem *ifJMP = listInsert(generateInstruction(I_RETURN,NULL,func->funcTable,NULL));
         }
         else
         {
            //printf("FUNCC RET TYPE %d\n",func->retType );
            if(func->retType == TYPE_STRING)
            {
               setRetType(TYPE_STRING);
            }
            if ((result = checkExpr())) { return result; }
            if (expression(false, func->stack)) { return SYNTAX_ERROR; }  
            if(func->retType == TYPE_STRING)
            {
               vysExp->type = TYPE_STRING;
            }
            tInstructionItem *ifJMP = listInsert(generateInstruction(I_RETURN,vysExp,func->funcTable,NULL));   
         }
         //printf("retasdasdurn\n");
              
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = veFunkci(func))) { return result; } 
         //printf("ret87878urn\n");

         return result; 
      }
      default:
         listInsert(generateInstruction(I_SKIP,NULL,NULL,NULL));
         return SUCCESS;
   }
}

int addParamToStack(tFunc *func, tStack *stack)
{
   int result;
   if (func->paramCnt)
   {
      tFuncParam *param = func->param;
      while (param)
      {
         tVar *var = calloc(1, sizeof(tVar));
         if (!var) { return INTERNAL_ERROR; }
         var->id = param->id;
         var->type = param->type;
         var->init = true;

         result = tsInsert(stack->table, var->id, var);

         if ((result == 1)) { return INTERNAL_ERROR; }
         if (result == 2) { debug("Redeklarace promenne '%s'\n", param->id); return SEM_ERROR; }
         debug("Parametr '%s' funkce '%s' pridan do TS funkce\n", var->id, func->name);

         param = param->nextParam;
      }
   }

   return SUCCESS;

}

int tsInsertFunction(table *TS, tFunc *func)
{
   int result;
   result = tsInsert(TS, func->name, func);
   if (result == 1) { return INTERNAL_ERROR; }
   debug("Funkce %s pridana do TS\n", func->name);

   tList *data = tsRead(TS, func->name);
   data->func = true;

   return SUCCESS;

}


// VSE OK -- HOTOVO
int compareFunction(tFunc *func1, tFunc *func2)
{
   if (func1->retType != func2->retType) { debug("Nerovna se navratovy typ\n"); return SYNTAX_ERROR; }
   if (func1->paramCnt != func2->paramCnt) { debug("Nerovna se pocet parametru\n"); return SYNTAX_ERROR; }
   else
   {
      tFuncParam *param1 = func1->param;
      tFuncParam *param2 = func2->param;

      while(param1)
      {

         //printf("%d %d \n", param1->type ,param2->type);
         if (param1->type != param2->type) { debug("Nerovna se typ parametru\n"); return SYNTAX_ERROR; }
         param1 = param1->nextParam;
         param2 = param2->nextParam;
      }
   }

   return SUCCESS;
}

// VSE OK -- HOTOVO
int dalsiParametr(tFunc *func)
{
   int result;

   switch (token.type)
   {
      case COMMA:
      {
         tType typ;
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } // ID
         if ((result = type(&typ))) { debug("spatny typ -- %s\n", printTok(&token)); return result; }
         if (typ == VOID) { debug("parametr typu VOID -- %s\n", printTok(&token)); return SEM_ERROR; }

         if (token.type != IDENTIFIER) { debug("nema ID -- %s\n", printTok(&token)); return result; }
         if (func)
            if (addFuncParam(func, typ)) { return INTERNAL_ERROR; }

         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } // ID
         if ((result = dalsiParametr(func))) { return result; }

         return result;
      }
      default:
         return SUCCESS;
   }
}

// VSE OK -- HOTOVO
int parametr(tFunc *func)
{
   int result;

   switch (token.type)
   {
      case RIGHT_BRACKET:
         return SUCCESS;

      case INT:
      case DOUBLE:
      case STRING:
      {
         tType typ = token.type;
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } // ID
         if (token.type != IDENTIFIER) { debug("nema ID -- %s\n", printTok(&token)); return result; }

         if (func)
            if (addFuncParam(func, typ)) { return INTERNAL_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } // ) nebo ,

         if ((result = dalsiParametr(func))) { return result; }
         if (token.type != RIGHT_BRACKET) { return SYNTAX_ERROR; } // )
         return result;

      }
      case VOID:
         debug("Void jako parametr funkce\n");
         return SEM_ERROR;

      default:
         break;
   }

   return SYNTAX_ERROR;
}

// VSE OK -- HOTOVO
int addFuncParam(tFunc *func, tType typ)
{
   tFuncParam *param = calloc(1, sizeof(tFuncParam));
   if(NULL == param) { return INTERNAL_ERROR; }

   func->paramCnt++;
   param->id = token.id;
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

int createFunc(tFunc **func, char *id, tType typ)
{
   if ( NULL == (*func = calloc(1, sizeof(tFunc)))) { return INTERNAL_ERROR; }
   (*func)->name = id;
   (*func)->retType = typ;
   (*func)->paramCnt = 0;
   (*func)->param = NULL;
   (*func)->className = NULL;
   (*func)->tsStack = NULL;
   (*func)->cnt = 0;
   return SUCCESS;
}

// VSE OK -- HOTOVO
int tsInsertVar(table *TS, tVar *var)
{
   int result = tsInsert(TS, var->id, var);
   if(result == 1) { debug("malloc err\n"); return INTERNAL_ERROR; } 
   if(result == 2) { debug("Redeklarace promenne %s.\n", var->id); return SEM_ERROR; }
   debug("Promenna %s byla pridana do TS\n", var->id);

   tList *data = tsRead(TS, var->id);
   data->func = false;
   
   return SUCCESS;
}

// VSE OK - HOTOVO
int createVar(tVar **var, char *id, tType typ, bool init)
{
   *var = calloc(1, sizeof(tVar));
   if (NULL == *var) { return INTERNAL_ERROR; }
   (*var)->id = id;
   (*var)->type = typ;
   (*var)->init = init;

   return SUCCESS;
}

tStack *createStack(tStack *stack, table *TS)
{
   tStack *new;
   if (NULL == (new = calloc(1, sizeof(tStack)))) { return NULL; }
   new->table = TS;
   new->next = stack;

   return new;
}


 
// VSE OK -- HOTOVO
int tsInsertClass(tClass **class)
{
   int result;
   tList *data = tsRead(globalTS, token.id);
   
   if (NULL == data)
   {
      // inicializuji data
      if (NULL == (*class = calloc(1, sizeof(tClass)))) { return INTERNAL_ERROR; }
      (*class)->name = token.id;

      if ((result = tsInsert(globalTS, (*class)->name, *class)) == 1) { return INTERNAL_ERROR; }
      debugClass("Class %s pridana do TS\n", (*class)->name);
   }
   else 
   { 
      debug("Trida %s jiz byla deklarovana\n", (*class)->name); 
      return SEM_ERROR;  // mozna jina chyba
   }

   return SUCCESS;
} 

// VSE OK -- HOTOVO
int type(tType *typ)
{
   int result;

   switch(token.type)
   {
      case INT:
      case DOUBLE:
      case STRING:
      case VOID:
         *typ = token.type;
         if((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         return result;
      default:
         break;
   }

   return SYNTAX_ERROR;
}

int controlMainRun()
{
   tList *data = tsRead(globalTS, "Main");
   if (!data) { debug("ERROR: class Main missing\n"); return SEM_ERROR; }
   else
   {
      tClass *class = data->dataPtr;
      data = tsRead(class->symbolTable, "run");
      if(!data) { debug("ERROR: static void run() in class Main missing\n"); return SEM_ERROR; }
      else if(!data->func) { debug("ERROR: static void run() : neni to funkce\n"); return SEM_ERROR; }
      tFunc *run = data->dataPtr;
      if(run->retType != TYPE_VOID) { debug("ERROR: static void run(): neni void\n"); return SEM_ERROR; }
      else if(run->paramCnt) { debug("ERROR: static void run(): nesouhlasi parametry\n"); return SEM_ERROR; }
   }
   return SUCCESS;
}


int GlobalWalkthrought()
{
   int result;

   tClass *class;
   tStack *stack;
   unsigned lCurlyBracket = 0; 
   unsigned rCurlyBracket = 0; 
   while (token.type != END_OF_FILE)
   {
      if (token.type == LEFT_CURLY_BRACKET)
         lCurlyBracket++;
      if (token.type == RIGHT_CURLY_BRACKET)
         rCurlyBracket++;
      if (token.type == CLASS)
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != IDENTIFIER) { return SYNTAX_ERROR; }
         if ((result = tsInsertClass(&class))) { return result; }
         if (NULL == (stack = createStack(NULL, class->symbolTable))) { return INTERNAL_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         continue;
      }

      if (token.type == STATIC && (lCurlyBracket-rCurlyBracket == 1))
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }

         if (token.type == INT || token.type == DOUBLE || token.type == STRING || token.type == VOID)
         {
            tType typ = token.type;
            if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
            if (token.type != IDENTIFIER) { return SYNTAX_ERROR; }
            char *id = token.id;
            if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
            if (token.type == LEFT_BRACKET)
            {
               tFunc *func;
               if (tsRead(class->symbolTable, id)) { return SEM_ERROR; } // redeklarace mozna jina chyba
               if ((result = createFunc(&func, id, typ))) { return result; }
               tsInit(func->funcTable);
               if (NULL == (func->stack = createStack(stack, func->funcTable))) { return INTERNAL_ERROR; }
               debug("Vytvoril sem tabulku symbolu pro funkci '%s'\n", func->name);
               if ((result = tsInsertFunction(class->symbolTable, func))) { return result; }
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
               if ((result = parametr(func))) { return result; }
               if ((result = addParamToStack(func, func->stack))) { return result; }
               continue;
            }
            if (token.type == ASSIGNMENT || token.type == SEMICOLON) 
            {
               if (typ == TYPE_VOID) { return SEM_ERROR; }
               tVar *var;
               if (createVar(&var, id, typ, false)) { return INTERNAL_ERROR; }
               if ((result = tsInsertVar(class->symbolTable, var))) { return result; }
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
               continue;
            }
            return SYNTAX_ERROR;

         }
         else 
         {
            return SYNTAX_ERROR;
         }
      }
      if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }

   }

   if (lCurlyBracket != rCurlyBracket) { return SYNTAX_ERROR; }
   return SUCCESS;

}


int parametrVolani(tFunc *func, tFunc *callFunc)
{
   int result;
   int i = 0;

   switch (token.type)
   {
      case IDENTIFIER:
      {
         tList *data = lookVarStack(func);
         if (!data) { return SYNTAX_ERROR; }
         if (data->func) { debug("Promenna je funkce\n"); return SYNTAX_ERROR; }
         tVar *var = data->dataPtr; 
         if (addFuncParam(callFunc, var->type)) { return INTERNAL_ERROR; }
         tokenParam[i] = var;

         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = dalsiParametrVolani(func, callFunc, ++i))) { return result; } 
         return result;
      }
      case FULL_IDENTIFIER:
      {
         char *className;
         char *functionName;
         functionName = parseFullId(&className, token.id);
         if (!functionName) { return SYNTAX_ERROR; }
         tList *data = tsRead(globalTS, className);
         if (!data) { debug("Nedeklarovana promena\n"); return SYNTAX_ERROR; }
         tClass *class = data->dataPtr;
         data = tsRead(class->symbolTable, functionName);
         if (!data) { debug("Nedeklarovana promena\n"); return SYNTAX_ERROR; }
         if (data->func) { debug("Promenna je funkce\n"); return SYNTAX_ERROR;}
         tVar *var = data->dataPtr; 
         if (addFuncParam(callFunc, var->type)) { return INTERNAL_ERROR; }

         tokenParam[i] = var;
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = dalsiParametrVolani(func, callFunc, ++i))) { return result; } 
         return result;
      }
      case CHAIN:
      {
        /////printf("toto %s \n",token.value.stringValue );
         if (addFuncParam(callFunc, TYPE_STRING)) { return INTERNAL_ERROR; }
         
         tVar *var = calloc(1, sizeof(tVar));
         if(!var) return INTERNAL_ERROR;
         var->type = TYPE_STRING;
         var->value.stringValue = token.id;
         var->init = true;
         tokenParam[i] = var;


         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = dalsiParametrVolani(func, callFunc, ++i))) { return result; } 
         return result;
      }
      case LIT_INT:
      {
         if (addFuncParam(callFunc, TYPE_INT)) { return INTERNAL_ERROR; }
         tVar *var = calloc(1, sizeof(tVar));
         if(!var) return INTERNAL_ERROR;
         var->type = TYPE_INT;
         var->value.intValue = atoi(token.id);
         var->init = true;
         tokenParam[i] = var;
       
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = dalsiParametrVolani(func, callFunc,++i))) { return result; } 
         return result;
      }

      case LIT_DOUBLE:
      
      {
         if (addFuncParam(callFunc, TYPE_DOUBLE)) { return INTERNAL_ERROR; }

          tVar *var = calloc(1, sizeof(tVar));
         if(!var) return INTERNAL_ERROR;
         var->type = TYPE_DOUBLE;
         char *ptr;
         var->value.doubleValue = strtod(token.id, &ptr);
         var->init = true;
         tokenParam[i] = var;


         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = dalsiParametrVolani(func, callFunc,++i))) { return result; } 
         return result;
      }
      default:
         return SUCCESS;
   }
}


int dalsiParametrVolani(tFunc *func, tFunc *callFunc, int i)
{
   int result;

   switch (token.type)
   {
      case COMMA:
      {

         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }

         switch(token.type)
         {
            case IDENTIFIER:
            {

               tList *data = lookVarStack(func);
               if (!data) { return SYNTAX_ERROR; }
               if (data->func) { debug("Promenna je funkce\n"); return SEM_ERROR; }
               tVar *var = data->dataPtr; 
               if (addFuncParam(callFunc, var->type)) { return INTERNAL_ERROR; }
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
               tokenParam[i] = var;
               if ((result = dalsiParametrVolani(func, callFunc, ++i))) { return result; }
               return result;
            }
            case FULL_IDENTIFIER:
            {
               char *className;
               char *functionName;
               functionName = parseFullId(&className, token.id);
               if (!functionName) { return SYNTAX_ERROR; }
               tList *data = tsRead(globalTS, className);
               if (!data) { debug("Nedeklarovana promena\n"); return SYNTAX_ERROR; }
               tClass *class = data->dataPtr;
               data = tsRead(class->symbolTable, functionName);
               if (!data) { debug("Nedeklarovana promena\n"); return SYNTAX_ERROR; }
               if (data->func) { debug("Promenna je funkce\n"); return SYNTAX_ERROR;}
               tVar *var = data->dataPtr; 
               if (addFuncParam(callFunc, var->type)) { return INTERNAL_ERROR; }
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
               tokenParam[i] = var;
               if ((result = dalsiParametrVolani(func, callFunc,++i))) { return result; }
               return result;
            }
            case CHAIN:
            {
              /////printf("toto %s \n",token.value.stringValue );
               if (addFuncParam(callFunc, TYPE_STRING)) { return INTERNAL_ERROR; }
               
               tVar *var = calloc(1, sizeof(tVar));
               if(!var) return INTERNAL_ERROR;
               var->type = TYPE_STRING;
               var->init = true;
               var->value.stringValue = token.id;
               tokenParam[i] = var;

               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
               if ((result = dalsiParametrVolani(func, callFunc, ++i))) { return result; } 
               return result;
            }
            case LIT_INT:
            
            {
               //printf("%d int \n",i );
               if (addFuncParam(callFunc, TYPE_INT)) { return INTERNAL_ERROR; }

               tVar *var = calloc(1, sizeof(tVar));
               if(!var) return INTERNAL_ERROR;
               var->type = TYPE_INT;
               var->init = true;
               var->value.intValue = atoi(token.id);
               tokenParam[i] = var;
              
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
               if ((result = dalsiParametrVolani(func, callFunc,++i))) { return result; } 
               return result;
            }

            case LIT_DOUBLE:
            
            {
               if (addFuncParam(callFunc, TYPE_DOUBLE)) { return INTERNAL_ERROR; }

               tVar *var = calloc(1, sizeof(tVar));
               if(!var) return INTERNAL_ERROR;
               var->type = TYPE_DOUBLE;
               var->init = true;
               char *ptr;
               var->value.doubleValue = strtod(token.id, &ptr);
               tokenParam[i] = var;
               
               

               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
               if ((result = dalsiParametrVolani(func, callFunc,++i))) { return result; } 
               return result;
            }
         }
                
      }
      default:
         return SUCCESS;
   }
}