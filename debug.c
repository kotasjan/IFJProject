#include "debug.h"
#include "scaner.h"
#include "ifj.h"
#include "ial.h"

void printErrCode(int code)
{
   switch (code)
   {
      case SUCCESS: break;     
      case LEX_ERROR: debug("LEX_ERROR - %d\n", code); break;     
      case SYNTAX_ERROR: debug("SYNTAX_ERROR - %d\n", code); break;      
      case SEM_ERROR: debug("SEM_ERROR - %d\n", code); break;      
      case SEM_TYPE_ERROR: debug("SEM_TYPE_ERROR - %d\n", code); break;      
      case SEM_OTHER_ERROR: debug("SEM_OTHER_ERROR - %d\n", code); break;       
      case INPUT_ERROR: debug("INPUT_ERROR - %d\n", code); break;      
      case UNINITIALIZED_ERROR: debug("UNINITIALIZED_ERROR - %d\n", code); break;  
      case DIV_ERROR: debug("DIV_ERROR - %d\n", code); break;      
      case OTHER_RNT_ERROR: debug("OTHER_RNT_ERROR - %d\n", code); break;       
      case INTERNAL_ERROR: debug("INTERNAL_ERROR - %d\n", code); break;      
   }
}

void printFunction(tFunc *func)
{
   debugFunc("Function name: %s\n", func->name);
   debugFunc("Function class name: %s\n", func->className);
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
            tClass *t = item->dataPtr;
            table *class = t->symbolTable;
            for(int j = 0; j < 100; j++)
            {

               if((*class)[j])
               {
                  if((*class)[j]->func){
                     
                     printf("Function: %s\n", (*class)[j]->key);
                     debugFunc("Function isFunc: %s\n", (*class)[j]->func ? "true" : "false");
                     printFunction((*class)[j]->dataPtr);
                  }
                  else
                  {
                     tVar *var = (*class)[j]->dataPtr;
                     printf("Variable: %s\n", (*class)[j]->key);
                     debugVar("Variable isFunc: %s\n", (*class)[j]->func ? "true" : "false");
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

char *printTok(tToken *token)
{
   switch(token->type)
   {
      case IDENTIFIER:
         return token->id;
         break;
      case FULL_IDENTIFIER:
         return token->id;
         break;
      case COMMA:     
         return ",";
         break;
      case SEMICOLON:
         return ";"; 
         break;
      case DOT:
         return ".";
         break;
      case END_OF_FILE:
         return "_EoF";
         break;
      case LEFT_BRACKET:
         return "(";
         break;
      case RIGHT_BRACKET:
         return ")";
         break;
      case LEFT_SQUARE_BRACKET:
         return "[";
         break;
      case RIGHT_SQUARE_BRACKET:
         return "]";
         break;
      case LEFT_CURLY_BRACKET:
         return "{";
         break;
      case RIGHT_CURLY_BRACKET:
         return "}";
         break;
      case PLUS: 
         return "+";
         break;
      case MINUS:
         return "-";
         break;
      case DIVISION:
         return "/";
         break;
      case MULTIPLIER:
         return "*";
         break;
      case INC: 
         return "++";
         break;
      case DEC: "--";
         return ;
         break;
      case GREATER:    
         return ">";
         break;     
      case NOT_EQUAL:
         return "!=";
         break;
      case EQUAL:
         return "==";
         break;
      case LESS:
         return "<";
         break;
      case GREATER_OR_EQUAL:
         return ">=";
         break;
      case LESS_OR_EQUAL:
         return "<=";
         break;
      case ASSIGNMENT:
         return "=";
         break;
      case EXCLAMATION:
         return "!";
         break;
      case INT:       
         return "int";
         break;
      case DOUBLE:
         return "double";
         break;
      case STRING:
         return token->id;
         break;
      case VOID:
         return "void";
         break;
      case BOOLEAN:
         return "boolen";
         break;
      case CHAIN:   
         return token->id;
         break;             
      case LIT_INT:
         return token->id;
         break;
      case LIT_DOUBLE:
         return token->id;
         break;
      case IF:     
         return "if";
         break;        
      case ELSE:
         return "else";
         break;
      case WHILE:
         return "while";
         break;
      case FOR:
         return "for";
         break;
      case DO:
         return "do";
         break;
      case BREAK:
         return "break";
         break;
      case CONTINUE:
         return "continue";
         break;
      case RETURN:
         return "return";
         break;
      case PRIVATE: 
         return "private";
         break;
      case PUBLIC:
         return "public";
         break;
      case MAIN:
         return "main";
         break;
      case CLASS:
         return "class";
         break;
      case STATIC:
         return "static";
         break;
      case FALSE:
         return "false";
         break;
      case TRUE:
         return "true";
         break;
   }
}

char giveTok(tToken *token){

switch(token->type)
   {
      case FULL_IDENTIFIER:
      case IDENTIFIER:
         return 'p';
      case CHAIN: 
         token->value.stringValue=token->id;  
         return 's';         
      case LIT_INT:
         token->value.intValue=atoi(token->id);
         return 'i';
      case LIT_DOUBLE:
      {
         char *ptr; 
         token->value.doubleValue=strtod(token->id,&ptr);
         return 'd';
      }
       case LEFT_BRACKET:
         return '(';
      case RIGHT_BRACKET:
         return ')';
      case PLUS: 
         return '+';
      case MINUS:
         return '-';
      case DIVISION:
         return '/';
      case MULTIPLIER:
         return '*';
      case GREATER_OR_EQUAL:
         return 'g';
      case LESS_OR_EQUAL:
         return 'o';
      case NOT_EQUAL:
         return 'n';
      case EQUAL:
         return 'e';
      case LESS:
         return '<';
      case GREATER:
         return '>';
      case SEMICOLON:
         return '$';
      default : return 'r';
   }
}
