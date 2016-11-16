#include "parse_new.h"
#include "ifj.h"
#include "ial.h"
#include "scaner.h"
#include "debug.h"
#include "ifj16Func.h"
#include <string.h>

table *globalTS = NULL;
tToken token;

int parse()
{
   int result;
   table TS;
   tsInit(&TS);
   globalTS = &TS;

   if ((result = addIFJ16(&TS))) { return result; }

   if ((result = getToken(&token))) { debug("%s\n", "Chyba v prvnim tokenu"); return result; }
   if ((result = prog(&TS))) { return result; }
   
   if (controlMainRun()) { return SYNTAX_ERROR; }

   printTS(globalTS);

   setFileToBegin(); // nastavi soubor na zacatek, 2. pruchod



}

int dalsiParametrVolani()
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
            case FULL_IDENTIFIER:
            case CHAIN:
            case LIT_INT:
            case LIT_DOUBLE:
               if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
               if ((result = dalsiParametrVolani())) { return result; }
               return result;
         }         
      }
      default:
         return SUCCESS;
   }
}

int parametrVolani()
{
   int result;

   switch (token.type)
   {
      case IDENTIFIER:
      case CHAIN:
      case LIT_INT:
      case LIT_DOUBLE:
      case FULL_IDENTIFIER:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = dalsiParametrVolani())) { return result; } 
         return result;
      }
      default:
         return SUCCESS;
   }
}

int rovnFun()
{
   int result;

   switch (token.type)
   {
      case LEFT_BRACKET:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = parametrVolani())) { return result; }
         if (token.type != RIGHT_BRACKET) { return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((token.type != SEMICOLON)) { return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         return result;
      }
      case ASSIGNMENT:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = expression())) { return result; }
         if ((token.type != SEMICOLON)) { return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         return result;
      }
      default:
         break;
   }

   return SYNTAX_ERROR;
}

int controlDecTable(table *TS, char *key)
{
   tList *data = tsRead(TS, key);
   if (data) { debug("Redeklarovana promena/ funkce %s\n", key); return SYNTAX_ERROR; }
      
   return SUCCESS;
}


int strRovn()
{
   int result;

   switch (token.type)
   {
      case SEMICOLON:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         return result;
      }
      case ASSIGNMENT:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = expression())) { return result; } 
         if (token.type != SEMICOLON) { return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         return result;
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

char *parseFullId(char **class)
{
   char *id = strtok(token.id, ".");
   bool inClass = true;
   while (id)
   {
      if(inClass) { *class = id; inClass = false; }
      else return id;

      id = strtok(NULL, ".");
   }

   return NULL;
}

int tsCheckVarFunc(table *TS, char *id, bool isFunc)
{
   tList *data = tsRead(TS, id);
   if (data)
   {
      if (data->func) { if (!isFunc) { debug("Promenna jiz byla deklarova jako funkce\n"); return SYNTAX_ERROR; } }
      else if (!data->func) { if (isFunc) { debug("Funkce jiz byla deklarova jako promenna\n"); return SYNTAX_ERROR; } }
   }
   else
   {
      if(isFunc)
      {
         tFunc *func = calloc(1, sizeof(tFunc));
         if(!func) { return INTERNAL_ERROR; }
         func->name = id;
         if (tsInsertFunction(TS, func)) { return INTERNAL_ERROR; }
      }
      else
      {
         tVar *var = calloc(1, sizeof(tVar));
         if(!var) { return INTERNAL_ERROR; }
         var->id = id;
         if (tsInsertVar(TS, var)) { return INTERNAL_ERROR; }

      }
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

int blok()
{
   int result;

   switch (token.type)
   {
      case IDENTIFIER:
      case FULL_IDENTIFIER:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = rovnFun())) { return result; }
         if ((result = blok())) { return result; } 
         return result;
      }
      case IF:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_BRACKET) { debug("IF nema ( -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = boolExp())) { return result; } 
         if (token.type != RIGHT_BRACKET) { debug("IF nema ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_CURLY_BRACKET) { debug("IF nema { -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok())) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("IF nema } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != ELSE) { debug("IF nema ELSE ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != LEFT_CURLY_BRACKET) { debug("IF nema { -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok())) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("IF nema } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = blok())) { return result; } 
         return result;
      }
      case DO:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_CURLY_BRACKET) { debug("DO nema { -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok())) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("DO nema } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != WHILE) { debug("DO nema WHILE ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_BRACKET) { debug("DO nema ( -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = boolExp())) { return result; } 
         if (token.type != RIGHT_BRACKET) { debug("DO nema ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != SEMICOLON) { debug("DO ; } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = blok())) { return result; } 
         return result;  
      }
      case WHILE:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_BRACKET) { debug("WHILE nema ( -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = boolExp())) { return result; } 
         if (token.type != RIGHT_BRACKET) { debug("WHILE nema ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != LEFT_CURLY_BRACKET) { debug("WHILE nema { -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok())) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("WHILE nema } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }        
         if ((result = blok())) { return result; } 
         return result; 
      }
      case RETURN:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = expression())) { return result; } 
         if (token.type != SEMICOLON) { debug("RETURN ; } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }        
         if ((result = blok())) { return result; } 
         return result; 
      }
      default:
         return SUCCESS;

   }
}

int veFunkci(tStack *stack)
{
   int result;

   switch (token.type)
   {
      case INT:
      case DOUBLE:
      case STRING:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != IDENTIFIER) { debug("Nema ID -- %s\n", printTok(&token)); return result; }
         if (controlDecTable(stack->table, token.id)) { return SYNTAX_ERROR; } // redeklarace promenne
         debug("Promenna %s byla pridana do TS funkce\n", token.id);
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = strRovn())) { return result; }
         if ((result = veFunkci(stack))) { return result; }
         return result;
      }
      case FULL_IDENTIFIER:
      case IDENTIFIER:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = rovnFun())) { return result; } 
         if ((result = veFunkci(stack))) { return result; } 
         return result;
      }
      case IF:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_BRACKET) { debug("IF nema ( -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = boolExp())) { return result; } 
         if (token.type != RIGHT_BRACKET) { debug("IF nema ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_CURLY_BRACKET) { debug("IF nema { -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok())) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("IF nema } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != ELSE) { debug("IF nema ELSE ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != LEFT_CURLY_BRACKET) { debug("IF nema { -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok())) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("IF nema } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = veFunkci(stack))) { return result; } 
         return result;
      }
      case DO:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_CURLY_BRACKET) { debug("DO nema { -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok())) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("DO nema } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != WHILE) { debug("DO nema WHILE ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_BRACKET) { debug("DO nema ( -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = boolExp())) { return result; } 
         if (token.type != RIGHT_BRACKET) { debug("DO nema ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != SEMICOLON) { debug("DO ; } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = veFunkci(stack))) { return result; } 
         return result;  
      }
      case WHILE:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if (token.type != LEFT_BRACKET) { debug("WHILE nema ( -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = boolExp())) { return result; } 
         if (token.type != RIGHT_BRACKET) { debug("WHILE nema ) -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != LEFT_CURLY_BRACKET) { debug("WHILE nema { -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = blok())) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { debug("WHILE nema } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }        
         if ((result = veFunkci(stack))) { return result; } 
         return result; 
      }
      case RETURN:
      {
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = expression())) { return result; } 
         if (token.type != SEMICOLON) { debug("RETURN ; } -- %s\n", printTok(&token)); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }        
         if ((result = veFunkci(stack))) { return result; } 
         return result; 
      }
      default:
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

         if (result == 1) { return INTERNAL_ERROR; }
         debug("Parametr funkce '%s' pridan do TS funkce\n", func->name);

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

int expression()
{
   int result;

   switch (token.type)
   {
      case END_OF_FILE:
         return SYNTAX_ERROR;
      default:
      {
         while(token.type != SEMICOLON)
         {
            if((result = getToken(&token))) { debug("%s\n", "ERROR - v LEX"); return result; }   
            if (token.type == END_OF_FILE) return SYNTAX_ERROR;
         }
         return result;
      }
   }
   return SYNTAX_ERROR;
}

int zavStrRov(table *TS, tType typ, char *id, tStack *prevStack)
{
   int result;

   switch (token.type)
   {
      // static int a;
      case SEMICOLON:   
      {
         // jedna se o promenou, nemuze byt void
         if (typ == TYPE_VOID) { debug("Promenna typu void\n"); return SEM_ERROR; }
         
         if (controlDecTable(TS, id)) { return SYNTAX_ERROR; } // redeklarace

         tVar *var; // inicializace dat
         if (createVar(&var, id, typ, false)) { return INTERNAL_ERROR; }
         if (tsInsertVar(TS, var)) { return INTERNAL_ERROR; } // pridani do TS
         debug("Promenna %s byla pridana do TS\n", id);

         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         return result;
      }
      // static int a(
      case LEFT_BRACKET:
      {

         if (controlDecTable(TS, id)) { return SYNTAX_ERROR; }

         tFunc *func;
         if (createFunc(&func, id, typ)) { return INTERNAL_ERROR; }

         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } // (
         if ((result = parametr(func))) { return result; }
         if (token.type != RIGHT_BRACKET) { return SYNTAX_ERROR; }   // )

         table table;
         tStack *stack = createStack(prevStack, &table); 
         if (addParamToStack(func, stack)) { return INTERNAL_ERROR; }
         if (tsInsertFunction(TS, func)) { return INTERNAL_ERROR; }

         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } // {
         if (token.type != LEFT_CURLY_BRACKET) { return SYNTAX_ERROR; } 
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } // telo nebo }
         if ((result = veFunkci(stack))) { return result; }

         if (token.type != RIGHT_CURLY_BRACKET) { return SYNTAX_ERROR; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 

         free(stack);
         prevStack->next = NULL;

         return result;         
      }
      case ASSIGNMENT:
      {
         // jedna se o promenou, nemuze byt void
         if (typ == TYPE_VOID) { debug("Promenna typu void\n"); return SEM_ERROR; }

         // redeklarace
         if (controlDecTable(TS, id)) { return SYNTAX_ERROR; }

         tVar *var; 
         if (createVar(&var, id, typ, true)) { return INTERNAL_ERROR; } // inicializace dat
         if (tsInsertVar(TS, var)) { return INTERNAL_ERROR; } // pridani do TS
         
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 
         if ((result = expression())) { debug("ERROR - v exp'\n"); return result; }
         if (token.type != SEMICOLON) { debug("chyby ;\n"); return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; } 

         return result;
      }



   }
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
         if(addFuncParam(func, typ)) { return INTERNAL_ERROR; }

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
   (*func)->isDeclared = true;

   return SUCCESS;
}

// VSE OK -- HOTOVO
int tsInsertVar(table *TS, tVar *var)
{
   int result = tsInsert(TS, var->id, var);
   if(result == 1) return INTERNAL_ERROR;
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
   tsInit(TS);
   tStack *new;
   if (NULL == (new = calloc(1, sizeof(tStack)))) { return NULL; }
   new->table = TS;
   new->next = stack;

   return new;
}

// VSE OK - HOTOVO
int prvkyTridy(table *TS, tStack *stack)
{
   int result;

   switch (token.type)
   {
      case STATIC:
      {
         tType typ;
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = type(&typ))) { return result; }
         if (token.type != IDENTIFIER) { debug("neni ID -- %s\n", printTok(&token)); return SYNTAX_ERROR; }
         char *id = token.id;
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = zavStrRov(TS, typ, id, stack))) { return result; }
         if ((result = prvkyTridy(TS, stack))) { return result; }
         return result;
      }
      default:
         return SUCCESS;
   }
}
 
// VSE OK - HOTOVO
int prog()
{
   int result;

   switch(token.type)
   {
      // EOF
      case END_OF_FILE: 
         return SUCCESS;


      // class ID { }
      case CLASS:
         if((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if(token.type != IDENTIFIER) { debug("%s\n", "ERROR: <class ID>"); return SYNTAX_ERROR; }

         // pridani tridy do tabulky
         tClass *class;
         if ((result = tsInsertClass(&class))) { return result; }

         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if (token.type != LEFT_CURLY_BRACKET) { debug("chyby { -- %s\n", printTok(&token)); return SYNTAX_ERROR; }

         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         
         tStack *stack;
         if (NULL == (stack = createStack(NULL, class->symbolTable))) { return INTERNAL_ERROR; }
         
         if ((result = prvkyTridy(class->symbolTable, stack))) { return result; }

         if (token.type != RIGHT_CURLY_BRACKET) { debug("chyby } -- %s\n", printTok(&token)); return SYNTAX_ERROR; }

         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         if ((result = prog())) { return result; }
         return SUCCESS;
      
      default:
         break;
   }

   return SYNTAX_ERROR;
}

// VSE OK -- HOTOVO
int tsInsertClass(tClass **class)
{
   int result;
   tList *data = tsRead(globalTS, token.id);
   
   // pokud jeste neni trida v tabulce symbolu z predchoziho volani, tak ji tam pridam
   if (NULL == data)
   {
      // inicializuji data
      if (NULL == (*class = calloc(1, sizeof(tClass)))) { return INTERNAL_ERROR; }
      (*class)->name = token.id;
      (*class)->isDeclared = true;

      if ((result = tsInsert(globalTS, (*class)->name, *class)) == 1) { return INTERNAL_ERROR; }
      debugClass("Class %s pridana do TS\n", (*class)->name);
   }
   // pokud jiz trida byla volana, nebo inicializovana
   else
   {
      (*class) = data->dataPtr;
      // redeklarace tridy
      if((*class)->isDeclared) { debug("Trida %s jiz byla deklarovana\n", (*class)->name); return SYNTAX_ERROR; }

      // trida se drive volala, ted je jeji definice
      else
      {
         (*class)->isDeclared = true;
      }
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
   if (!data) { debug("ERROR: class Main missing\n"); return SYNTAX_ERROR; }
   else
   {
      tClass *class = data->dataPtr;
      data = tsRead(class->symbolTable, "run");
      if(!data) { debug("ERROR: static void run() in class Main missing\n"); return SYNTAX_ERROR; }
      else if(!data->func) { debug("ERROR: static void run() : neni to funkce\n"); return SYNTAX_ERROR; }
      tFunc *run = data->dataPtr;
      if(run->retType != TYPE_VOID) { debug("ERROR: static void run(): neni void\n"); return SYNTAX_ERROR; }
      else if(run->paramCnt) { debug("ERROR: static void run(): nesouhlasi parametry\n"); return SYNTAX_ERROR; }
   }
   return SUCCESS;
}
