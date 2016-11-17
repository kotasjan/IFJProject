#include "sem.h"
#include "ifj.h"
#include "ial.h"
#include "scaner.h"
#include "debug.h"
#include "interpret.h"
#include "instruction.h"
#include "parse_new.h"
#include <string.h>
#include <stdlib.h>

tToken token;

tClass *getTS()
{
   int result;
   tList *data = tsRead(globalTS, token.id);
   tClass *class = data->dataPtr;
   return class;
}
int semStart(table *TS)
{
   int result;
   getToken(&token); // ID
   if ((result = semProg(TS))) { return result; }
   return SUCCESS;
}

int semProg(table *TS)
{
   int result;

   switch (token.type)
   {
      case END_OF_FILE: ;return SUCCESS;

      case CLASS: 
      {
         getToken(&token); // ID
         tClass *class = getTS();
         tStack *stack;
         if (NULL == (stack = createStack(NULL, class->symbolTable))) { return INTERNAL_ERROR; }
         getToken(&token); // {
         getToken(&token); // 
         if ((result = semPrvkyTridy(stack))) { return result; }

         getToken(&token); // 
         if ((result = semProg(TS))) { return result; }

         return SUCCESS;
      }
      default:
         break;
   }
   return SYNTAX_ERROR;
}

int semPrvkyTridy(tStack *stack)
{
   int result;

   switch (token. type)
   {
      case STATIC:
      {
         tType typ;
         getToken(&token); // typ
         if ((result = semType(&typ))) { return result; }
         char *id = token.id;
         getToken(&token); // 
         if ((result = semZavStrRov(stack, typ, id))) { return result; }
         if ((result = semPrvkyTridy(stack))) { return result; }
         return SUCCESS;
      }
   }


}

int tsInitVar(table *table, char *id)
{
   int result;
   tList *data = tsRead(table, id);
   if (!data) { return NULL; }
   if (data->func) { return NULL; }
   tVar *var = data->dataPtr;
   var->init = true;

   return data->dataPtr;
}

int semDalsiParametr()
{
   int result;

   switch (token.type)
   {
      case COMMA:
      {
         getToken(&token); // ID
         tType typ;
         if ((result = semType(&typ))) { debug("spatny typ -- %s\n", printTok(&token)); return result; }
         getToken(&token); // ID
         if ((result = semDalsiParametr())) { return result; }
         return SUCCESS;
      }
      default:
         return SUCCESS;
   }

}

int semParametr()
{
   int result;

   switch (token.type)
   {
      case RIGHT_BRACKET: return SUCCESS;

      case INT:
      case DOUBLE:
      case STRING:
      {
         getToken(&token); // ID
         getToken(&token); //
         if ((result = semDalsiParametr())) { return result; }
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

int semExpression(tStack *stack)
{

}

int semStrRovn()
{
   int result;

   switch (token.type)
   {
      case SEMICOLON:
      {
         getToken(&token);
         return result;
      }
      case ASSIGNMENT:
      {
         getToken(&token);
         if ((result = expression())) { return result; } 
         getToken(&token);
         return result;
      }
      default:
         break;
   }

   return SYNTAX_ERROR;

}

int semVeFunkci(tStack *stack)
{
   int result;

   switch (token.type)
   {
      case INT:
      case DOUBLE:
      case STRING:
      {
         getToken(&token);
         char id = token.id;
         getToken(&token);
         if ((result = semStrRovn())) { return result; }
      }
   }

   return SUCCESS;
}

int semZavStrRov(tStack *stack, tType typ, char *id)
{
   int result;

   switch (token.type)
   {
      case SEMICOLON:
      {
         getToken(&token);
         return SUCCESS;
      }
      case LEFT_BRACKET:
      {
         getToken(&token);
         if ((result = semParametr())) { return result; }

         tList *data = tsRead(stack->table, id);
         if (!data) { return SYNTAX_ERROR; }
         tFunc *func = data->dataPtr;

         table table;
         tsInit(&table);
         tStack *newStack = createStack(stack, &table);
         if (addParamToStack(func, stack)) { return INTERNAL_ERROR; }
         getToken(&token);
         getToken(&token);
         if ((result = semVeFunkci(stack))) { return result; }


         // TODO
      }
      case ASSIGNMENT:
      {
         if ((result = expression(stack))) { debug("ERROR - v exp'\n"); return result; }
         tVar *var;
         if (NULL == (var = tsInitVar(stack->table, id))) { return SYNTAX_ERROR; }

         tInstruction *inst;
        // if (NULL == (inst = createInst(I_ASSIGNMENT, hodnotaodsekyho, NULL, var))) { return INTERNAL_ERROR; }

         getToken(&token);
         return SUCCESS;
      }
   }

}

int semType(tType *typ)
{
   int result;

   switch (token.type)
   {
      case INT:
      case DOUBLE:
      case STRING:
      case VOID:
      {
         *typ = token.type;
         getToken(&token); // 
         return SUCCESS;
      }    
      default:
         break;
   }
   return SYNTAX_ERROR;
}