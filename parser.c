#include "parser.h"
#include "ifj.h"
#include "ial.h"
#include "scaner.h"
#include "debug.h"
#include <string.h>

tToken token;

table *global = NULL;

typedef struct stack
{
   table *table;
   struct stack *next;
} tStack;



void printFunction(tFunc *func)
{
   debugFunc("Function name: %s\n", func->name);
   debugFunc("Function retType: %s\n", func->retType == TYPE_INT ? "int" : func->retType == TYPE_DOUBLE ? "double" : func->retType == TYPE_STRING ? "String" : "void" );
   debugFunc("Function paramCnt: %u\n", func->paramCnt);
   tFuncParam *tmp = func->param;
   int i = 0;
   while(tmp)
   {
      debugFunc("Function param %d: %s %s\n", i++, \
         tmp->type == TYPE_INT ? "int" : tmp->type == TYPE_DOUBLE ? "double" : tmp->type == TYPE_STRING ? "string" : "void", tmp->id);
      tmp = tmp->nextParam;
   }
}

void printTS(table *TS)
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
            tClass *t = item->classData;
            if(!t->isDeclared){
               printf("neni deklarovana class %s\n",(*TS)[i]->key );
               exit(1);
            }
            table *class = t->symbolTable;
            for(int j = 0; j < 100; j++)
            {

               if((*class)[j])
               {
                  if((*class)[j]->func){
                     
                     printf("Function: %s\n", (*class)[j]->key);
                     printFunction((*class)[j]->classData);
                  }
                  else
                  {
                     tVar *var = (*class)[j]->classData;
                     printf("Variable: %s\n", (*class)[j]->key);
                     debugVar("Variable Name: %s\n", var->id);
                     debugVar("Variable type: %s\n", var->type == TYPE_INT ? "int" : var->type == TYPE_DOUBLE ? "double" : var->type == TYPE_STRING ? "string" : "void" );
                     debugVar("Variable init: %s\n", var->init ? "true" : "false");
                  }

               }
            }
            item = item->next;
         }

      }

   }

}

int controlMainRun(table *TS)
{
   tList *data = tsRead(TS, "Main");
   if (!data) { debug("ERROR: class Main missing\n"); return SYNTAX_ERROR; }
   else
   {
      tClass *class = data->classData;
      data = tsRead(class->symbolTable, "run");
      if(!data) { debug("ERROR: static void run() in class Main missing\n"); return SYNTAX_ERROR; }
      else if(!data->func) { debug("ERROR: static void run() : neni to funkce\n"); return SYNTAX_ERROR; }
      tFunc *run = data->classData;
      if(run->retType != TYPE_VOID) { debug("ERROR: static void run(): neni void\n"); return SYNTAX_ERROR; }
      else if(run->paramCnt) { debug("ERROR: static void run(): nesouhlasi parametry\n"); return SYNTAX_ERROR; }
   }

   return SUCCESS;
}



int addClass(table *TS, tClass **class)
{
   int result;
   tList *data = tsRead(TS, token.id);
   if(!data)
   {
      debug("Definice tridy - prvni volani\n"); 
      *class = calloc(1, sizeof(tClass));
      if(!*class) { debug("%s\n", "malloc failed"); return INTERNAL_ERROR; }
      (*class)->name = token.id;
      (*class)->isDeclared = true;

      result = tsInsert (TS, token.id, *class);
      if(result == 1) return INTERNAL_ERROR;
      else if(result == 2) { debug("%s %s\n", "Redifined class", "");  return SEM_ERROR; }
      else debugClass("%s %s %s\n", "class", (*class)->name, "added to ST");

   }
   else
   {
      *class = data->classData;
      if((*class)->isDeclared) 
      { 
         debug("Redifined class\n"); 
         return SYNTAX_ERROR; 
      }
      else
      {
         debug("Trida jiz byla volana, ted se deklaruje\n");
         (*class)->isDeclared = true;
      }

   }
   

   return SUCCESS;

}

int parse()
{

   int result;
   table TS;
   tsInit(&TS);
   global = &TS;

   result = addIFJ16(&TS);

   if ((result = getToken(&token)) != SUCCESS ) { debug("%s\n", "Chyba v prvnim tokenu"); return result; }
   else { debug("%s\n", "Zacinam program <prog>"); }

   if ((result = prog(&TS))) { return result; }

   if((result = controlMainRun(&TS))) { return result; }

   printTS(&TS);

   return result;
} 

int prog(table *TS)
{
   int result;
   debug("%s\n", "<prog>");

   switch(token.type)
   {
      case END_OF_FILE: return SUCCESS;

      case CLASS:

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != IDENTIFIER) { debug("%s\n", "ERROR: <class ID>"); return SYNTAX_ERROR; }

         // pridani tridy do tabulky         
         tClass *class;
         if ((result = addClass(TS, &class)) != SUCCESS ) { return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_CURLY_BRACKET) { debug("%s %s\n", "<prvky tridy> - chyby {", printTok(&token)); return SYNTAX_ERROR; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = prvky_tridy(class->symbolTable))) { return result; }

         if(token.type != RIGHT_CURLY_BRACKET) { debug("%s %s\n", "<prvky tridy> - chyby }", printTok(&token)); return SYNTAX_ERROR; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         debug("<class ID> - \"%s\" \n", printTok(&token));
         if((result = prog(TS))) { return result; }

         break;
      
      default:
         break;
     }

}


int prvky_tridy(table *TS)
{
   int result;
   debug("%s\n", "<prvky tridy>");

   switch (token.type)
   {
      case STATIC:
         debug("%s %s\n", "<prvky tridy>", printTok(&token)); // {
         varType type;
      
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = typ(&type))) { return result; }

         if(token.type != IDENTIFIER) { debug("%s %s\n", "<static typ ID> - neni ID", printTok(&token)); return SYNTAX_ERROR; }
         char *id = token.id;

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = zavStrRov(TS, type, id))) { return result; }

         //result = funcOrVar();
         result = prvky_tridy(TS);
         return result;
         break;
      default:

         return SUCCESS;
   }

}


int typ(varType *type)
{
   int result;
   debug("%s\n", "<typ>");

   switch(token.type)
   {
      case INT:
      case DOUBLE:
      case STRING:
      case VOID:
         debug("%s %s\n", "<typ>", printTok(&token)); 
         *type = token.type;
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         return result;

      default:
         break;

   }

   return SYNTAX_ERROR;
}

int zavStrRov(table *TS, varType type, char *key)
{
   int result;
   debug("%s\n", "<zavStrRov>"); 

   switch(token.type)
   {
      case SEMICOLON:;
      	if (type==TYPE_VOID){
      		debug("%s %s\n", "Promenna je typu void", ""); return SEM_ERROR;
      	}

         tList *data = tsRead(TS, key);
         if(data)
         {
            tVar *var = data->classData;
            if(!var->isDeclared)
               debug("Deklarace promenne, jiz byla volana\n");
            else
            {
               debug("%s %s\n", "Redifined var", ""); return SEM_ERROR;
            }
         }
         else{

         // pridani promenne
         //**********************************************//
         tVar *var = calloc(1, sizeof(tVar));
         if(!var) { return INTERNAL_ERROR; }
         var->id = key;
         var->type = type;
         var->init = false;

         result = tsInsert (TS, var->id,var);
         if(result == 1) return INTERNAL_ERROR;
         else if(result == 2) { debug("%s %s\n", "Redifined var", ""); return SEM_ERROR; }
         else debug("%s %s %s\n", "variable", var->id, "added to ST");

         tList *data = tsRead(TS, var->id);
         if(data) data->func = false;
      }
         //**********************************************//

         debug("%s %s\n", "<zavStrRov> deklarace", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         return result;

      case LEFT_BRACKET:;

         //**********************************************//
         //               pridani funkce                 //
         //**********************************************//
         tFunc *func = calloc(1, sizeof(tFunc));
         if(!func) { return INTERNAL_ERROR; }
         func->name = key;
         func->retType = type;
         //**********************************************//

         debug("%s %s\n", "<zavStrRov> funkce", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = parametr(func))) { return result; }
         if(token.type != RIGHT_BRACKET) { return SYNTAX_ERROR; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_CURLY_BRACKET) { return SYNTAX_ERROR; } // muze to byt deklarace funkce

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }


         // vytvoreni zasobniku tabulky symbolu
         tStack *stack = calloc(1, sizeof(tStack));
         if(!stack) {return INTERNAL_ERROR;}
         stack->table = TS;

         tStack *stack1 = calloc(1,sizeof(tStack));
         if(!stack1) { return INTERNAL_ERROR; }
         table table;
         memset(&table, 0, sizeof(table)); 
         stack1->table = &table;
         stack1->next = stack;
         printf("vytvarim dalsi uroven\n");

         if((result = veFunkci(stack1))) { return result; }
         if(token.type != RIGHT_CURLY_BRACKET) { return SYNTAX_ERROR; }

         //**********************************************//
         //               pridani funkce                 //
         //**********************************************//
         result = tsInsert (TS, func->name, func);
         if(result == 1) return INTERNAL_ERROR;
         else if(result == 2) { debug("%s %s\n", "Redifined func", ""); return SEM_ERROR; }
         else debug("%s %s %s\n", "func", func->name, "added to ST");

         data = tsRead(TS, func->name);
         if(data) data->func = true;
         //**********************************************//
         //**********************************************//

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }

         return result;


      case ASSIGNMENT:;

      	 if (type==TYPE_VOID){
      			debug("%s %s\n", "Promenna je typu void", ""); return SEM_ERROR;
      		}
         // pridani promenne
         //**********************************************//
         tVar *var = calloc(1, sizeof(tVar));
         if(!var) { return INTERNAL_ERROR; }
         var->id = key;
         var->type = type;
         var->init = true;

         result = tsInsert (TS, var->id,var);
         if(result == 1) return INTERNAL_ERROR;
         else if(result == 2) { debug("%s %s\n", "Redifined var", ""); return SEM_ERROR; }
         else debug("%s %s %s\n", "variable", var->id, "added to ST");

         debugVar("Variable Name: %s\n", var->id);
         debugVar("Variable type: %s\n", var->type == TYPE_INT ? "int" : var->type == TYPE_DOUBLE ? "double" : var->type == TYPE_STRING ? "string" : "void" );
         debugVar("Variable init: %s\n", var->init ? "true" : "false");
         data = tsRead(TS, var->id);
         if(data) data->func = false;
         //**********************************************//


         debug("%s %s\n", "<zavStrRov> vyraz", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = item())) { return result; }

         if(token.type  != SEMICOLON) { return SYNTAX_ERROR; }
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }

         return result;

      default:
         break;
   }

   return SYNTAX_ERROR;

}

int strRovn()
{
   int result;
   debug("%s\n", "<strRovn>");

   switch(token.type)
   {
      case SEMICOLON:
         debug("%s %s\n", "<strRovn> deklarace", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         return result;

      case ASSIGNMENT:
         debug("%s %s\n", "<strRovn> definice", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = item())) { return result; }
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         return result;



         // TODO
         //IDEN PLNY IDE

   }

}

int dalsiParametrVolani()
{
   int result;
   debug("%s\n", "<dalsiParametrVolani>"); 

   switch(token.type)
   {
      case COMMA:
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != IDENTIFIER) { debug("%s %s\n", "<dalsiParametrVolani> ID - neni ID", printTok(&token)); return SYNTAX_ERROR; }
         debug("%s %s\n", "<dalsiParametrVolani> ID ", printTok(&token));

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = dalsiParametrVolani())) { return result; } 
         return result;

      default:
         return SUCCESS;
   }

}

int parametrVolani()
{
   int result;
   debug("%s\n", "<parametrVolani>"); 

   switch(token.type)
   {
      case IDENTIFIER:
         debug("%s %s\n", "<parametrVolani> ID ", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = dalsiParametrVolani())) { return result; } 
         return result;

      default:
         return SUCCESS;


   }


}

int rovnFun1(table *TS, char *key, bool declared)
{

   int result;
   debug("%s\n", "<rovnFun__1>");  

   switch(token.type)
   {
      case ASSIGNMENT:
         debug("%s %s\n", "<rovnFun__1> prirazeni", printTok(&token));

         tList *data = tsRead(TS, key);
         if(!data)
         {          
            debug("promenna jeste nebyla deklarovana\n");
            tVar *var = calloc(1, sizeof(tVar));
            var->id = key;
            var->init = false;  // todo
            var->isDeclared = false;

            result = tsInsert (TS, key, var);
            if(result == 1) return INTERNAL_ERROR;
            else if(result == 2) { debug("%s %s\n", "Redifined class", "");  return SEM_ERROR; }
            else debugClass("%s %s %s\n", "var", var->id, "added to ST"); 
         }
   
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = item())) { return result; }
         if(token.type != SEMICOLON) { debug("%s %s\n", "<rovnFun><item> chybi ; ", printTok(&token)); return result; }
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         return result;

      case LEFT_BRACKET:
         debug("%s %s\n", "<rovnFun__1> funkce", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }

         if((result = parametrVolani())) { return result; }
         if(token.type != RIGHT_BRACKET) { debug("%s %s\n", "<rovnFun><funkce> chybi ) ; ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != SEMICOLON) { debug("%s %s\n", "<rovnFun><funkce> chybi ; ; ", printTok(&token)); return result; } 

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         return result; 

            
   }


}


int rovnFun()
{
   int result;
   debug("%s\n", "<rovnFun>");  

   switch(token.type)
   {
      case ASSIGNMENT:
         debug("%s %s\n", "<rovnFun> prirazeni", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = item())) { return result; }
         if(token.type != SEMICOLON) { debug("%s %s\n", "<rovnFun><item> chybi ; ", printTok(&token)); return result; }
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         return result;

      case LEFT_BRACKET:
         debug("%s %s\n", "<rovnFun> funkce", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }

         if((result = parametrVolani())) { return result; }
         if(token.type != RIGHT_BRACKET) { debug("%s %s\n", "<rovnFun><funkce> chybi ) ; ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != SEMICOLON) { debug("%s %s\n", "<rovnFun><funkce> chybi ; ; ", printTok(&token)); return result; } 

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         return result;    
            
   }


}

int veFunkci(tStack *stack)
{
   int result;
   debug("%s\n", "<veFunkci>");

   switch(token.type)
   {
      case INT:
      case DOUBLE:
      case STRING:
         debug("%s %s\n", "<veFunkci> typ", printTok(&token));

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != IDENTIFIER) { debug("%s %s\n", "<veFunkci><typ> ID - nema ID ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = strRovn())) { return result; }
         if((result = veFunkci(stack))) { return result; }
         return result; 

      case IDENTIFIER:
           debug("%s %s\n", "<veFunkci> ID", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = rovnFun())) { return result; } 

          if((result = veFunkci(stack))) { return result; } 

          return result;
      case FULL_IDENTIFIER:;

      	char *tmp = strtok(token.id, ".");
         int i = 0;
         tClass *class = NULL;
         bool declared = false;
      	while(tmp) {
            
            tList *data;
            if(!i)
            {
               data = tsRead(global, tmp);
               if(!data)
               {
                  debug("Volani tridy %s.. jeste nebyla deklarovana\n", tmp); 
                  class = calloc(1, sizeof(tClass));
                  if(!class) { debug("%s\n", "malloc failed"); return INTERNAL_ERROR; }
                  class->name = tmp;
                  class->isDeclared = false;

                  result = tsInsert (global, tmp, class);
                  if(result == 1) return INTERNAL_ERROR;
                  else if(result == 2) { debug("%s %s\n", "Redifined class", "");  return SEM_ERROR; }
                  else debugClass("%s %s %s\n", "class", class->name, "added to ST");

                  data = tsRead(global, tmp);
                  class = data->classData;
               }
               else{ 
                  
                  class = data->classData;
                  if(class->isDeclared) declared = true;
               }
            }
            else
            {
               debug("%s %s\n", "<veFunkci> FULL ID", printTok(&token));
               if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }

               if((result = rovnFun1(class->symbolTable, tmp, declared))) { return result; } 
/*
               result = tsInsert (class->symbolTable, tmp, );
               if(result == 1) return INTERNAL_ERROR;
               else if(result == 2) { debug("%s %s\n", "Redifined class", "");  return SEM_ERROR; }
               else debugClass("%s %s %s\n", "class", class->name, "added to ST"); 
*/
            }
            i++;
            
            
        	
        	tmp = strtok(NULL, ".");
    		}

    		
         

          if((result = veFunkci(stack))) { return result; } 

          return result;

      case IF:
         debug("%s %s\n", "<veFunkci> IF", printTok(&token));

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_BRACKET) { debug("%s %s\n", "<veFunkci> IF chybi ( ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = expression())) { return result; } 
         if(token.type != RIGHT_BRACKET) { debug("%s %s\n", "<veFunkci> IF chybi ) ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_CURLY_BRACKET) { debug("%s %s\n", "<veFunkci> IF chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = blok(stack))) { return result; } 
         if(token.type != RIGHT_CURLY_BRACKET) { debug("%s %s\n", "<veFunkci> IF chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != ELSE) { debug("%s %s\n", "<veFunkci> IF chybi ELSE ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_CURLY_BRACKET) { debug("%s %s\n", "<veFunkci> IF chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = blok(stack))) { return result; } 
         if(token.type != RIGHT_CURLY_BRACKET) { debug("%s %s\n", "<veFunkci> IF chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = veFunkci(stack))) { return result; } 

         return result;

      case DO:
         debug("%s %s\n", "<veFunkci> DO", printTok(&token));

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_CURLY_BRACKET) { debug("%s %s\n", "<veFunkci> DO chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = blok(stack))) { return result; } 
         if(token.type != RIGHT_CURLY_BRACKET) { debug("%s %s\n", "<veFunkci> DO chybi } ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != WHILE) { debug("%s %s\n", "<veFunkci> DO chybi WHILE ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_BRACKET) { debug("%s %s\n", "<veFunkci> DO chybi ( ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = expression())) { return result; } 
         if(token.type != RIGHT_BRACKET) { debug("%s %s\n", "<veFunkci> DO chybi ) ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != SEMICOLON) { debug("%s %s\n", "<veFunkci> DO chybi ; ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = veFunkci(stack))) { return result; } 

         return result;

      case WHILE:
         debug("%s %s\n", "<veFunkci> WHILE", printTok(&token));

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_BRACKET) { debug("%s %s\n", "<veFunkci> WHILE chybi ( ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = expression())) { return result; } 
         if(token.type != RIGHT_BRACKET) { debug("%s %s\n", "<veFunkci> WHILE chybi ) ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_CURLY_BRACKET) { debug("%s %s\n", "<veFunkci> WHILE chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = blok(stack))) { return result; } 
         if(token.type != RIGHT_CURLY_BRACKET) { debug("%s %s\n", "<veFunkci> WHILE chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = veFunkci(stack))) { return result; } 

         return result;

      case RETURN:
         debug("%s %s\n", "<veFunkci> RETURN", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = expression())) { return result; } 

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = veFunkci(stack))) { return result; } 

         return result;

      default:
         return SUCCESS;
   }



}

int blok(tStack *stack)
{
   int result;
   debug("%s\n", "<blok>");
   tStack *stack1 = calloc(1,sizeof(tStack));
   if(!stack1) { return INTERNAL_ERROR; }
   table table;
   memset(&table, 0, sizeof(table)); 
   stack1->table = &table;
   stack1->next = stack;
   printf("vytvarim dalsi uroven\n");

   
   int i = 0;
   tStack *tmp = stack;
   bool finded = false;
   while(tmp){
      tList *data = tsRead(tmp->table, "vyraz");
      if(!data){ 
         printf("nenasel sem: uroven %d\n",i );
         tmp = tmp->next;
         }
      else{
         finded = true;
         printf("nasel sem: uroven %d\n",i);
         break;
      }
      i++;

   }
   if(!finded) { debug("UNDEFINED REFERENCE\n"); return SYNTAX_ERROR; }
   

   switch(token.type)
   {
      case IDENTIFIER:
      case FULL_IDENTIFIER:
      	 debug("%s %s\n", "<blok> ID", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = rovnFun())) { return result; } 

          if((result = blok(stack1))) { return result; } 

          return result;
         

      case IF:
         debug("%s %s\n", "<blok> IF", printTok(&token));

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_BRACKET) { debug("%s %s\n", "<blok> IF chybi ( ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = expression())) { return result; } 
         if(token.type != RIGHT_BRACKET) { debug("%s %s\n", "<blok> IF chybi ) ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_CURLY_BRACKET) { debug("%s %s\n", "<blok> IF chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = blok(stack1))) { return result; } 
         if(token.type != RIGHT_CURLY_BRACKET) { debug("%s %s\n", "<blok> IF chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != ELSE) { debug("%s %s\n", "<veFunkci> IF chybi ELSE ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_CURLY_BRACKET) { debug("%s %s\n", "<blok> IF chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = blok(stack1))) { return result; } 
         if(token.type != RIGHT_CURLY_BRACKET) { debug("%s %s\n", "<blok> IF chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = blok(stack1))) { return result; }

         return result;

       case WHILE:
         debug("%s %s\n", "<veFunkci> WHILE", printTok(&token));

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_BRACKET) { debug("%s %s\n", "<veFunkci> WHILE chybi ( ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = expression())) { return result; } 
         if(token.type != RIGHT_BRACKET) { debug("%s %s\n", "<veFunkci> WHILE chybi ) ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_CURLY_BRACKET) { debug("%s %s\n", "<veFunkci> WHILE chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = blok(stack1))) { return result; } 
         if(token.type != RIGHT_CURLY_BRACKET) { debug("%s %s\n", "<veFunkci> WHILE chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = blok(stack1))) { return result; }

         return result;


        case DO:
         debug("%s %s\n", "<veFunkci> DO", printTok(&token));

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_CURLY_BRACKET) { debug("%s %s\n", "<veFunkci> DO chybi { ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = blok(stack1))) { return result; } 
         if(token.type != RIGHT_CURLY_BRACKET) { debug("%s %s\n", "<veFunkci> DO chybi } ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != WHILE) { debug("%s %s\n", "<veFunkci> DO chybi WHILE ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != LEFT_BRACKET) { debug("%s %s\n", "<veFunkci> DO chybi ( ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = expression())) { return result; } 
         if(token.type != RIGHT_BRACKET) { debug("%s %s\n", "<veFunkci> DO chybi ) ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != SEMICOLON) { debug("%s %s\n", "<veFunkci> DO chybi ; ", printTok(&token)); return result; }

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = blok(stack1))) { return result; }
         return result;
   }


   return SUCCESS;

}

int expression()
{
   int result;
   debug("%s\n", "<expression>"); 

   switch(token.type)
   {
      default:;
         // docasne - simulace expression
         int bracket = 1;
         while(token.type != RIGHT_BRACKET)
         {
            if(token.type == LEFT_BRACKET) { bracket++; }

            if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }   
            if(token.type == RIGHT_BRACKET) {
               while(token.type == RIGHT_BRACKET){
                  bracket--; 
                  if(bracket) if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; } 
                  if(!bracket) break;
               }  
            }    
         }
         return result;
      //expression TODO

   }

}

int item()
{
   int result;
   debug("%s\n", "<item>"); 

   switch(token.type)
   {
      /*
      case IDENTIFIER:
         debug("%s %s\n", "<item> item->ID", printTok(&token));
         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         return result;
      */
      default:
         // docasne - simulace expression
         while(token.type != SEMICOLON)
         {
            if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }            
         }
         return result;
      //expression TODO

   }

}

int parametr(tFunc *func)
{
   int result;
   debug("%s\n", "<parametr>"); 

   switch(token.type)
   {
      case RIGHT_BRACKET:
         return SUCCESS;

      default:;
         varType type;
         if((result = typ(&type))) { debug("%s %s\n", "<parametr> <typ> - spatny typ", printTok(&token)); return result; }

         tFuncParam *param = calloc(1, sizeof(tFuncParam));
         if(!param) { return INTERNAL_ERROR; }

         func->paramCnt++;
         func->param = param;

         if(token.type != IDENTIFIER) { debug("%s %s\n", "<parametr><typ> ID - nema ID ", printTok(&token)); return result; }
         debug("%s %s\n", "<parametr><typ> ID = ", printTok(&token));

         func->param->id = token.id;
         func->param->type = type;

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }

         if((result = dalsiParametr(func, param))) { return result; }
         if(token.type != RIGHT_BRACKET) { return SYNTAX_ERROR; }
         debug("%s %s\n", "ukonceni <parametr> ", printTok(&token));

         return result;


   }

}

int dalsiParametr(tFunc *func, tFuncParam *param)
{
   int result;
   debug("%s\n", "<dalsiParametr>"); 

   switch(token.type)
   {
      case COMMA:;
         varType type;

         tFuncParam *new = calloc(1, sizeof(tFuncParam));
         if(!new) { return INTERNAL_ERROR; }
         param->nextParam = new;

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = typ(&type))) { debug("%s %s\n", "<parametr> <typ> - spatny typ", printTok(&token)); return result; }

         new->type = type;

         //if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if(token.type != IDENTIFIER) { debug("%s %s\n", "Parmanetr nena ID", printTok(&token)); return SYNTAX_ERROR; }

         new->id = token.id;
         func->paramCnt++;

         debug("%s %s\n", "<parametr> <typ> ID - ", printTok(&token));

         if((result = getToken(&token)) != SUCCESS) { debug("%s\n", "ERROR - v LEX"); return result; }
         if((result = dalsiParametr(func, new)) != SUCCESS) { return result; }

         return result;

      default:
         return SUCCESS;
   }

}

int addIFJ16(table *TS)
{
   int result;

   tClass *trida = calloc(1, sizeof(tClass));
   if(trida == NULL){
      debug("%s\n", "calloc failed");
      return INTERNAL_ERROR;
   }

   trida->isDeclared = true;

   result = tsInsert (TS, "ifj16", trida);
   if(result == 1) return INTERNAL_ERROR;
   else if(result == 2) { debug("%s %s\n", "Redifined class", ""); return SEM_ERROR; }
   else debugClass("%s\n", "class ifj16 added to ST");

   tList *ifj16 = tsRead(TS, "ifj16");
   if(!ifj16) { return INTERNAL_ERROR; }

   // readInt
   tFunc *func = calloc(1, sizeof(tFunc));
   if(!func) { return INTERNAL_ERROR; }
   func->name = "readInt";
   func->retType = TYPE_INT;
   result = tsInsert (ifj16->classData, func->name, func);
   if(result == 1) return INTERNAL_ERROR;
   else if(result == 2) { debug("%s %s\n", "Redifined func", ""); return SEM_ERROR; }
   else debug("%s %s %s\n", "func", func->name, "added to ST");

   tList *data = tsRead(ifj16->classData, func->name);
   if(data) data->func = true;

   // readDouble
   func = calloc(1, sizeof(tFunc));
   if(!func) { return INTERNAL_ERROR; }
   func->name = "readDouble";
   func->retType = TYPE_DOUBLE;
   result = tsInsert (ifj16->classData, func->name, func);
   if(result == 1) return INTERNAL_ERROR;
   else if(result == 2) { debug("%s %s\n", "Redifined func", ""); return SEM_ERROR; }
   else debug("%s %s %s\n", "func", func->name, "added to ST");

   data = tsRead(ifj16->classData, func->name);
   if(data) data->func = true;

   // readString
   func = calloc(1, sizeof(tFunc));
   if(!func) { return INTERNAL_ERROR; }
   func->name = "readString";
   func->retType = TYPE_STRING;
   result = tsInsert (ifj16->classData, func->name, func);
   if(result == 1) return INTERNAL_ERROR;
   else if(result == 2) { debug("%s %s\n", "Redifined func", ""); return SEM_ERROR; }
   else debug("%s %s %s\n", "func", func->name, "added to ST");

   data = tsRead(ifj16->classData, func->name);
   if(data) data->func = true;

   // print
   // TODO parametry
   func = calloc(1, sizeof(tFunc));
   if(!func) { return INTERNAL_ERROR; }
   func->name = "print";
   func->retType = TYPE_VOID;
   result = tsInsert (ifj16->classData, func->name, func);
   if(result == 1) return INTERNAL_ERROR;
   else if(result == 2) { debug("%s %s\n", "Redifined func", ""); return SEM_ERROR; }
   else debug("%s %s %s\n", "func", func->name, "added to ST");

   data = tsRead(ifj16->classData, func->name);
   if(data) data->func = true;

   // length
   func = calloc(1, sizeof(tFunc));
   if(!func) { return INTERNAL_ERROR; }
   func->name = "length";
   func->retType = TYPE_INT;

   tFuncParam *param = calloc(1, sizeof(tFuncParam));
   if(!param) { return INTERNAL_ERROR; }

   func->paramCnt++;
   func->param = param;
   func->param->id = "s";
   func->param->type = TYPE_STRING;

   result = tsInsert (ifj16->classData, func->name, func);
   if(result == 1) return INTERNAL_ERROR;
   else if(result == 2) { debug("%s %s\n", "Redifined func", ""); return SEM_ERROR; }
   else debug("%s %s %s\n", "func", func->name, "added to ST");

   data = tsRead(ifj16->classData, func->name);
   if(data) data->func = true;

   // substr
   func = calloc(1, sizeof(tFunc));
   if(!func) { return INTERNAL_ERROR; }
   func->name = "substr";
   func->retType = TYPE_STRING;

   tFuncParam *ptr;
   param = calloc(1, sizeof(tFuncParam));
   if(!param) { return INTERNAL_ERROR; }

   ptr = param;
   func->paramCnt++;
   func->param = param;
   func->param->id = "s";
   func->param->type = TYPE_STRING;

   param = calloc(1, sizeof(tFuncParam));
   if(!param) { return INTERNAL_ERROR; }

   ptr->nextParam = param;
   ptr = param;

   func->paramCnt++;
   param->id = "i";
   param->type = TYPE_INT;

   param = calloc(1, sizeof(tFuncParam));
   if(!param) { return INTERNAL_ERROR; }

   ptr->nextParam = param;
   ptr = param;

   func->paramCnt++;
   param->id = "n";
   param->type = TYPE_INT;

   result = tsInsert (ifj16->classData, func->name, func);
   if(result == 1) return INTERNAL_ERROR;
   else if(result == 2) { debug("%s %s\n", "Redifined func", ""); return SEM_ERROR; }
   else debug("%s %s %s\n", "func", func->name, "added to ST");

   data = tsRead(ifj16->classData, func->name);
   if(data) data->func = true;


   // compare
   func = calloc(1, sizeof(tFunc));
   if(!func) { return INTERNAL_ERROR; }
   func->name = "compare";
   func->retType = TYPE_INT;

   param = calloc(1, sizeof(tFuncParam));
   if(!param) { return INTERNAL_ERROR; }

   ptr = param;
   func->paramCnt++;
   func->param = param;
   func->param->id = "s1";
   func->param->type = TYPE_STRING;

   param = calloc(1, sizeof(tFuncParam));
   if(!param) { return INTERNAL_ERROR; }

   ptr->nextParam = param;
   ptr = param;

   func->paramCnt++;
   param->id = "s2";
   param->type = TYPE_STRING;

   result = tsInsert (ifj16->classData, func->name, func);
   if(result == 1) return INTERNAL_ERROR;
   else if(result == 2) { debug("%s %s\n", "Redifined func", ""); return SEM_ERROR; }
   else debug("%s %s %s\n", "func", func->name, "added to ST");

   data = tsRead(ifj16->classData, func->name);
   if(data) data->func = true;

   // find
   func = calloc(1, sizeof(tFunc));
   if(!func) { return INTERNAL_ERROR; }
   func->name = "find";
   func->retType = TYPE_INT;

   param = calloc(1, sizeof(tFuncParam));
   if(!param) { return INTERNAL_ERROR; }

   ptr = param;
   func->paramCnt++;
   func->param = param;
   func->param->id = "s";
   func->param->type = TYPE_STRING;

   param = calloc(1, sizeof(tFuncParam));
   if(!param) { return INTERNAL_ERROR; }

   ptr->nextParam = param;
   ptr = param;

   func->paramCnt++;
   param->id = "search";
   param->type = TYPE_STRING;

   result = tsInsert (ifj16->classData, func->name, func);
   if(result == 1) return INTERNAL_ERROR;
   else if(result == 2) { debug("%s %s\n", "Redifined func", ""); return SEM_ERROR; }
   else debug("%s %s %s\n", "func", func->name, "added to ST");

   data = tsRead(ifj16->classData, func->name);
   if(data) data->func = true;

   // sort
   func = calloc(1, sizeof(tFunc));
   if(!func) { return INTERNAL_ERROR; }
   func->name = "sort";
   func->retType = TYPE_STRING;

   param = calloc(1, sizeof(tFuncParam));
   if(!param) { return INTERNAL_ERROR; }

   ptr = param;
   func->paramCnt++;
   func->param = param;
   func->param->id = "s";
   func->param->type = TYPE_STRING;

   result = tsInsert (ifj16->classData, func->name, func);
   if(result == 1) return INTERNAL_ERROR;
   else if(result == 2) { debug("%s %s\n", "Redifined func", ""); return SEM_ERROR; }
   else debug("%s %s %s\n", "func", func->name, "added to ST");

   data = tsRead(ifj16->classData, func->name);
   if(data) data->func = true;

}
