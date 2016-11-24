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
tInstructionList *GlobalList;




int semStart()
{

   debug("Zacatek semanticke analyzi\n");
   int result;
   getToken(&token); // ID

   GlobalList = listInit();

   tInstr *inst = generateInstruction(I_PUSH_GLOBAL, NULL, NULL, NULL);
   if (!inst) debug("GEN. inst err malloc");

   tInstructionItem *item = listInsert(inst);
   if (!item) debug("LIST item error");
   debug("Vygeneroval jsem instrukci global.\n");

   if ((result = semProg())) { return result; }
   return SUCCESS;
}

int semProg()
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
         if ((result = semProg())) { return result; }

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

int semRovnFun()
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
         if ((result = expressionSkip())) { return result; }
         if ((token.type != SEMICOLON)) { return result; }
         if ((result = getToken(&token))) { debug("ERROR - v LEX\n"); return result; }
         return result;
      }
      default:
         break;
   }

   return SYNTAX_ERROR;
}

int semExpression(tStack *stack)
{

}

int semStrRovn(tType typ, char *id, tStack *stack)
{
   int result;

   switch (token.type)
   {
      case SEMICOLON:
      {
         tVar *var;
         if (createVar(&var, id, typ, false)) { return INTERNAL_ERROR; }
         if ((result = tsInsertVar(stack->table, var))) { return result; }
         getToken(&token);
         return result;
      }
      case ASSIGNMENT:
      {
         tVar *var;
         if (createVar(&var, id, typ, false)) { return INTERNAL_ERROR; }
         if ((result = tsInsertVar(stack->table, var))) { return result; }
         tToken tmp;
         memcpy(&tmp, &token, sizeof(tToken));
         if (token.type == IDENTIFIER || token.type == FULL_IDENTIFIER)
         {
            getToken(&token);
            if (token.type != LEFT_BRACKET)
            {
               if (insertTokenFifo(&tmp)) { return INTERNAL_ERROR; }
               if (insertTokenFifo(&token)) { return INTERNAL_ERROR; }
               getToken(&token);
               if ((result = expression(false))) { return result; } 
            }
            else
            {
               getToken(&token);
               if ((result = parametrVolani())) { return result; }
               getToken(&token);
            }
         }
         else
         {
            getToken(&token);
            if ((result = expression(false))) { return result; } 
         }

         if (token.type != SEMICOLON) { return SYNTAX_ERROR; }
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
         tType typ = token.type;
         getToken(&token);
         char *id = token.id;
         getToken(&token);
         if ((result = semStrRovn(typ, id, stack))) { return result; }
         if (( result = semVeFunkci(stack))) { return result; }
         return result;
      }
      case IDENTIFIER:
      {
         tList *data = NULL;
         tStack *tmp = stack;
         while (!data)
         {
            printf("1\n");
            printf("%s\n",token.id );
            data = tsRead(tmp->table, token.id);
            if (!data) tmp = tmp->next;  
            if (data)
            {
               printf("konec\n");
               break;
            }
            if (!tmp) { debug("nedeklarovana\n"); return SYNTAX_ERROR; }
            
         }
         printf("5454\n");
         getToken(&token);
         semRovnFun();
      }
      case FULL_IDENTIFIER:
      {
         // TODO
      }


      default:
         return SUCCESS;
   }

   return SUCCESS;
}

int semZavStrRov(tStack *stack, tType typ, char *id)
{
   int result;

   switch (token.type)
   {
      // static int a;
      case SEMICOLON:
      {
         getToken(&token);
         return SUCCESS;
      }
      // static void run ()
      case LEFT_BRACKET:
      {
         getToken(&token);
         if ((result = semParametr())) { return result; }

         tList *data = tsRead(stack->table, id);
         if (!data) { return SYNTAX_ERROR; }
         tFunc *func = data->dataPtr;

         printFunction(func);

         table table;
         tsInit(&table);
         tStack *newStack = createStack(stack, &table);
         if (addParamToStack(func, newStack)) { return INTERNAL_ERROR; }
         getToken(&token);
         getToken(&token);
         if ((result = semVeFunkci(newStack))) { return result; }
         if (token.type != RIGHT_CURLY_BRACKET) { return SYNTAX_ERROR; }
         getToken(&token);
         return result;

         // TODO
      }
      // static int a = 
      case ASSIGNMENT:
      {

         if ((result = expression(stack))) { debug("ERROR - v exp'\n"); return result; }
         tVar *var;
         if (NULL == (var = tsInitVar(stack->table, id))) { return SYNTAX_ERROR; }
         debug("Promenna '%s' byla inicializovana\n", var->id);

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