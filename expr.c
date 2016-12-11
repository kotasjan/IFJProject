#include "ial.h"
#include "scaner.h"
#include "expr.h"
#include "debug.h"
#include "ifj.h"
#include "parse_new.h"
#include <stdbool.h>

tExpStack *zasobnik;
tToken token;
tStack *stack;
tValueStack *top;
tToken *vysExp;

tType retType = TYPE_UNDEF;

void setRetType(tType typ)
{
   retType = typ;
}


int onlyOne = 0;

tList *lookVarStackExp()
{
   tList *data = tsRead(stack->table, token.id);
   if (data) 
   { 
      debug("nasel sem promenou '%s'\n", token.id);  
      if (data->func) { debug("Funkce ve vyrazu"); return NULL; }
      return data; 
   }
   if (stack->next)
   {
      data = tsRead(stack->next->table, token.id);
      if (data) 
      { 
         debug("nasel sem promenou '%s'\n", token.id); 
         if (data->func) { debug("Funkce ve vyrazu"); return NULL; } 
         return data; 
      }
      debug("Promenna '%s' neni deklarovana\n", token.id);
   }
   else
   {
      debug("Promenna '%s' neni deklarovana\n", token.id);
   }
   return NULL;
}

struct conStr
{
   unsigned alloc;
   unsigned len;
   char * str;
   bool concat;
   bool plus;
} conStr;

int checkSizeStr(int length)
{
   if (conStr.len+length+1 >= conStr.alloc)
   {
      while ( conStr.len+length+1 >= conStr.alloc )
      {
         conStr.alloc += 20;
      }
      conStr.str = realloc (conStr.str, sizeof(char) * (conStr.alloc) );
      if (conStr.str == NULL)
      {
         return -1;
      }
      
   }
   return 0;
}

int addConCat() 
{
   if (conStr.concat)
   {
      if (checkSizeStr(strlen(token.id))) return INTERNAL_ERROR;
      sprintf(&conStr.str[conStr.len], "%s", token.id); 
      conStr.len += strlen(token.id);
     // printf("%s\n",conStr.str );
   }

   return SUCCESS;
}

int getVal(tValueStack **val){

   switch(token.type)
   {
      case LIT_INT: 
         if (addConCat()) return INTERNAL_ERROR;
         (*val)->value.intValue=token.value.intValue; 
         debug("Int cislo %d\n", (*val)->value.intValue);
         (*val)->typ = TYPE_INT;
         (*val)->var = NULL;
         return SUCCESS;

      case LIT_DOUBLE:  
         if (addConCat()) return INTERNAL_ERROR;
         (*val)->value.doubleValue=token.value.doubleValue; 
         debug("Double cislo %f\n", (*val)->value.doubleValue);
         (*val)->typ = TYPE_DOUBLE;
         (*val)->var = NULL;
         return SUCCESS;

      case CHAIN: 
         if (addConCat()) return INTERNAL_ERROR;
         (*val)->value.stringValue=token.value.stringValue; 
         debug("String %s\n", (*val)->value.stringValue);
         (*val)->typ = TYPE_STRING;
         (*val)->var = NULL;
         if (retType == TYPE_DOUBLE)
         {
            debug("STRING V OPREaci s INT\n");
               exit(4);
         }
         return SUCCESS;

      case IDENTIFIER:
      {
         tList *data = lookVarStackExp();
         if (!data) { return SEM_ERROR; }
         tVar *var = data->dataPtr;
         (*val)->typ = var->type;
         (*val)->var = var;
         if (retType != TYPE_UNDEF)
         {
            if ((var->type == TYPE_STRING) && (retType != TYPE_STRING))
            {
               debug("STRING V OPREaci s INT double\n");
               exit(4);
            }
            if ((var->type == TYPE_DOUBLE) && (retType == TYPE_INT))
            {
               debug("DOUBLE V OPREaci s INT\n");
               exit(4);
            }
         }
         // if (!var->init) { debug("Neinicializovana promenna ve vyrazu\n"); return UNINITIALIZED_ERROR; } // asi jina chyba
         return SUCCESS;
      }
      
      case FULL_IDENTIFIER:
      {
         char *className;
         char *id;
         id = parseFullId(&className, token.id);
         if (!id) return INTERNAL_ERROR;
         tList *data = tsRead(globalTS, className);
         if (!data) { debug("Nedeklarovana funkce/promenna\n"); return SEM_ERROR; }
         tClass *class = data->dataPtr;
         data = tsRead(class->symbolTable, id);
         if (!data) { debug("Nedeklarovana funkce/promenna\n"); return SEM_ERROR; }
         if (data->func) { debug("funkce ve vyrazu\n"); return SEM_ERROR; } 
         tVar *var = data->dataPtr;
         (*val)->typ = var->type;
         (*val)->var = var;
         if (retType != TYPE_UNDEF)
         {
            if ((var->type == TYPE_STRING) && (retType != TYPE_STRING))
            {
               debug("STRING V OPREaci s INT double\n");
               exit(4);
            }
            if ((var->type == TYPE_DOUBLE) && (retType == TYPE_INT))
            {
               debug("DOUBLE V OPREaci s INT\n");
               exit(4);
            }
         }

         return SUCCESS;
         //if (!var->init) { debug("Neinicializovana promenna\n"); return UNINITIALIZED_ERROR; } // asi jina chyba

      }
      

      case MINUS:
      case MULTIPLIER:
      case DIVISION:
         conStr.concat = false;
         (*val)->typ = token.type;
         (*val)->var = NULL;
         return SUCCESS;
      case PLUS:
         conStr.plus = true;
         (*val)->typ = token.type;
         (*val)->var = NULL;
         return SUCCESS;

      case LESS:
      case NOT_EQUAL:
      case EQUAL:
      case GREATER_OR_EQUAL:
      case LESS_OR_EQUAL:
      case GREATER:
         conStr.concat = false;
         (*val)->typ = token.type;
         (*val)->var = NULL;
         return SUCCESS;
      default:
         break;

   }

   return -1;
}

int push(char c, int typ, tExpStack *pom)
{
   tExpStack *tmp = (tExpStack*) malloc(sizeof(tExpStack));
   if (!tmp) { return INTERNAL_ERROR; }

   switch (typ)
   {
      case PUSH_NEW:
      {
         tValueStack *val = (tValueStack*) malloc(sizeof(tValueStack));
         if (!val) return INTERNAL_ERROR;
         int res = getVal(&val);
         if(res == -1)
         {
            free(val);
            //printf("FREE \n\n\n\n\n");
            
         }
         if(res > 0)
         {
            return SEM_ERROR; // predelat chybu 
         }
         
         else if (res != -1)
         {
            val->next = top;
            top = val;
         }

      }      
      case PUSH_REDUCE:
      {
         tmp->next=zasobnik;
         tmp->data=c;
         zasobnik=tmp;
         break;
      }
      case PUSH_END:
      {
         tmp->data=c;
         tmp->next=NULL;
         zasobnik = tmp;
         break;
      }
      case PUSH_STOP:
      {
         tmp->data=c;
         tmp->next=pom;
         if(pom==(zasobnik)) zasobnik=tmp;
         else (zasobnik)->next=tmp;
         break;
      }
   }

   return SUCCESS;
}

int controlType(oper, typ)
{
   switch (oper)
   {
      case '+':
      {
         if (vysExp->type == TYPE_UNDEF) { vysExp->type = typ; return vysExp->type; }
         if (vysExp->type == TYPE_INT && typ == TYPE_INT)       return vysExp->type;
         if (vysExp->type == TYPE_DOUBLE && typ == TYPE_DOUBLE) return vysExp->type;
         if (vysExp->type == TYPE_DOUBLE && typ == TYPE_INT)    return vysExp->type;
         if (vysExp->type == TYPE_STRING && typ == TYPE_STRING) return vysExp->type;
         if (vysExp->type == TYPE_STRING && typ == TYPE_DOUBLE) return vysExp->type;
         if (vysExp->type == TYPE_STRING && typ == TYPE_INT)    return vysExp->type;
         if (vysExp->type == TYPE_INT && typ == TYPE_DOUBLE)    { vysExp->type = TYPE_DOUBLE; return vysExp->type; }
         if (vysExp->type == TYPE_INT && typ == TYPE_STRING)    { vysExp->type = TYPE_STRING; return vysExp->type; }
         if (vysExp->type == TYPE_DOUBLE && typ == TYPE_STRING) { vysExp->type = TYPE_STRING; return vysExp->type; }
      }
      case '-':
      case '*':
      case '/':
      {
         if (vysExp->type == TYPE_UNDEF) { vysExp->type = typ; return vysExp->type; }
         if (vysExp->type == TYPE_INT && typ == TYPE_INT)       return vysExp->type;
         if (vysExp->type == TYPE_DOUBLE && typ == TYPE_DOUBLE) return vysExp->type;
         if (vysExp->type == TYPE_DOUBLE && typ == TYPE_INT)    return vysExp->type;
         if (vysExp->type == TYPE_STRING && typ == TYPE_STRING) return TYPE_UNDEF;
         if (vysExp->type == TYPE_STRING && typ == TYPE_DOUBLE) return TYPE_UNDEF;
         if (vysExp->type == TYPE_STRING && typ == TYPE_INT)    return TYPE_UNDEF;
         if (vysExp->type == TYPE_INT && typ == TYPE_STRING)    return TYPE_UNDEF;
         if (vysExp->type == TYPE_DOUBLE && typ == TYPE_STRING) return TYPE_UNDEF; 
         if (vysExp->type == TYPE_INT && typ == TYPE_DOUBLE)    { vysExp->type = TYPE_DOUBLE; return vysExp->type; }
      }  
      case '<':  // <         
      case '>':  // >
      case 'E':  // ==
      case '!':  // !=
      case 'G':  // >=
      case 'L':  // <= 
      {
         if (vysExp->type == TYPE_UNDEF) { vysExp->type = typ; return vysExp->type; }
         if (vysExp->type == TYPE_INT && typ == TYPE_INT)       return vysExp->type;
         if (vysExp->type == TYPE_DOUBLE && typ == TYPE_DOUBLE) return vysExp->type;
         if (vysExp->type == TYPE_DOUBLE && typ == TYPE_INT)    return vysExp->type;
         if (vysExp->type == TYPE_STRING && typ == TYPE_STRING) return TYPE_UNDEF;
         if (vysExp->type == TYPE_STRING && typ == TYPE_DOUBLE) return TYPE_UNDEF;
         if (vysExp->type == TYPE_STRING && typ == TYPE_INT)    return TYPE_UNDEF;
         if (vysExp->type == TYPE_INT && typ == TYPE_STRING)    return TYPE_UNDEF;
         if (vysExp->type == TYPE_DOUBLE && typ == TYPE_STRING) return TYPE_UNDEF; 
         if (vysExp->type == TYPE_INT && typ == TYPE_DOUBLE)    { vysExp->type = TYPE_DOUBLE; return vysExp->type; }
      }   
   }
}

int calcVal(char oper, tType typ)
{
   tInstructionItem *item = NULL;

   tValueStack *val = (tValueStack*) malloc(sizeof(tValueStack));
   if (!val) return INTERNAL_ERROR;

   val->next = top->next->next->next;

   val->typ = typ;

   switch (oper)
   {
      case '-':
      {
         switch (controlType(oper, typ))
         {
            case TYPE_UNDEF:
            {
               debug("Nekompatibilni datove type %s - %s\n", typ == TYPE_STRING ? "String" : typ == TYPE_INT ? "int" : "double",
                                           vysExp->type == TYPE_STRING ? "String" : vysExp->type == TYPE_INT ? "int" : "double");
               return SEM_ERROR;
            }
            
            case TYPE_INT: 
               if (!top->next->next->var && !top->var)
               {
                  //val->value.intValue = top->next->next->value.intValue - top->value.intValue;
                  item = listInsert(generateInstruction(I_MINUS, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                  //val->value.intValue = ((tVar*)top->next->next->var)->value.intValue - top->value.intValue; 
                  item = listInsert(generateInstruction(I_MINUS_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                 // val->value.intValue = top->next->next->value.intValue - ((tVar*)top->var)->value.intValue; 
                  item = listInsert(generateInstruction(I_MINUS_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                  //val->value.intValue = ((tVar*)top->next->next->var)->value.intValue - ((tVar*)top->var)->value.intValue;
                  item = listInsert(generateInstruction(I_MINUS_BOTH, top->next->next->var, top->var, val)); 
               }
                  
               
               debug("Vytvarim instrukci I_MINUS (INT)\n");
               //if (!item)// printf("CHYBA EXPR -\n");
            break;
            case TYPE_DOUBLE: 
               if (!top->next->next->var && !top->var)
               {
                  //val->value.doubleValue = top->next->next->value.doubleValue - top->value.doubleValue;
                  item = listInsert(generateInstruction(I_MINUS, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                  //val->value.doubleValue = ((tVar*)top->next->next->var)->value.doubleValue - top->value.doubleValue; 
                  item = listInsert(generateInstruction(I_MINUS_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                  //val->value.doubleValue = top->next->next->value.doubleValue - ((tVar*)top->var)->value.doubleValue; 
                  item = listInsert(generateInstruction(I_MINUS_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                 // val->value.doubleValue = ((tVar*)top->next->next->var)->value.doubleValue - ((tVar*)top->var)->value.doubleValue;
                  item = listInsert(generateInstruction(I_MINUS_BOTH, top->next->next->var, top->var, val)); 
               }

               val->value.doubleValue = top->next->next->value.doubleValue - top->value.doubleValue;
               item = listInsert(generateInstruction(I_MINUS, top->next->next, top, val));
               debug("Vytvarim instrukci I_MINUS (DOUBLE)\n");
               //if (!item) printf("CHYBA EXPR -\n");
            break;

            case TYPE_STRING:
               break;
         }
         break;
      }
      case '+':
      {

         switch (controlType(oper, typ))
         {
            case TYPE_UNDEF:
            {
               debug("Nekompatibilni datove type %s - %s\n", typ == TYPE_STRING ? "String" : typ == TYPE_INT ? "int" : "double",
                                           vysExp->type == TYPE_STRING ? "String" : vysExp->type == TYPE_INT ? "int" : "double");
               return SEM_ERROR;
            }
            case TYPE_INT:
               if (!top->next->next->var && !top->var)
               {
                  //printf("TOTOTTOTOPPPP|n\n\n\n\n %d\n", top->next->next->typ);
                  //val->value.intValue = top->next->next->value.intValue + top->value.intValue;
                  item = listInsert(generateInstruction(I_ADD, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                 // val->value.intValue = ((tVar*)top->next->next->var)->value.intValue + top->value.intValue; 
                  item = listInsert(generateInstruction(I_ADD_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                 // val->value.intValue = top->next->next->value.intValue + ((tVar*)top->var)->value.intValue; 
                  item = listInsert(generateInstruction(I_ADD_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                 // val->value.intValue = ((tVar*)top->next->next->var)->value.intValue + ((tVar*)top->var)->value.intValue;
                  item = listInsert(generateInstruction(I_ADD_BOTH, top->next->next->var, top->var, val)); 
               }

               debug("Vytvarim instrukci I_PLUS (INT)\n");
               break;
            case TYPE_DOUBLE: 
               if (!top->next->next->var && !top->var)
               {
                 // val->value.doubleValue = top->next->next->value.doubleValue + top->value.doubleValue;
                  item = listInsert(generateInstruction(I_ADD, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                 // val->value.doubleValue = ((tVar*)top->next->next->var)->value.doubleValue + top->value.doubleValue; 
                  item = listInsert(generateInstruction(I_ADD_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                 // val->value.doubleValue = top->next->next->value.doubleValue + ((tVar*)top->var)->value.doubleValue; 
                  item = listInsert(generateInstruction(I_ADD_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                  //val->value.doubleValue = ((tVar*)top->next->next->var)->value.doubleValue + ((tVar*)top->var)->value.doubleValue;
                  item = listInsert(generateInstruction(I_ADD_BOTH, top->next->next->var, top->var, val)); 
               }
               debug("Vytvarim instrukci I_PLUS (DOUBLE)\n");
               break;
            case TYPE_STRING:
               break;
         }
         break;
      }
      case '/':
      {
         switch (controlType(oper, typ))
         {

            case TYPE_UNDEF:
            {
               debug("Nekompatibilni datove type %s - %s\n", typ == TYPE_STRING ? "String" : typ == TYPE_INT ? "int" : "double",
                                           vysExp->type == TYPE_STRING ? "String" : vysExp->type == TYPE_INT ? "int" : "double");
               return SEM_ERROR;
            }

            case TYPE_INT:
               if (!top->next->next->var && !top->var)
               {
                 // val->value.intValue = top->next->next->value.intValue / top->value.intValue;
                  item = listInsert(generateInstruction(I_DIV, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                 // val->value.intValue = ((tVar*)top->next->next->var)->value.intValue / top->value.intValue; 
                  item = listInsert(generateInstruction(I_DIV_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                 // val->value.intValue = top->next->next->value.intValue / ((tVar*)top->var)->value.intValue; 
                  item = listInsert(generateInstruction(I_DIV_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                 // val->value.intValue = ((tVar*)top->next->next->var)->value.intValue / ((tVar*)top->var)->value.intValue;
                  item = listInsert(generateInstruction(I_DIV_BOTH, top->next->next->var, top->var, val)); 
               }

               debug("Vytvarim instrukci I_DIV (INT)\n");
               break;
            case TYPE_DOUBLE: 
               if (!top->next->next->var && !top->var)
               {
                  //val->value.doubleValue = top->next->next->value.doubleValue / top->value.doubleValue;
                  item = listInsert(generateInstruction(I_DIV, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                  //val->value.doubleValue = ((tVar*)top->next->next->var)->value.doubleValue / top->value.doubleValue; 
                  item = listInsert(generateInstruction(I_DIV_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                  //val->value.doubleValue = top->next->next->value.doubleValue / ((tVar*)top->var)->value.doubleValue; 
                  item = listInsert(generateInstruction(I_DIV_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                  //val->value.doubleValue = ((tVar*)top->next->next->var)->value.doubleValue / ((tVar*)top->var)->value.doubleValue;
                  item = listInsert(generateInstruction(I_DIV_BOTH, top->next->next->var, top->var, val)); 
               }         
               debug("Vytvarim instrukci I_DIV (DOUBLE)\n"); 

            break;
            case TYPE_STRING:
               break;
         }
         break;
      }
      case '*':
      {
         switch (controlType(oper, typ))
         {
            case TYPE_UNDEF:
            {
               debug("Nekompatibilni datove type %s - %s\n", typ == TYPE_STRING ? "String" : typ == TYPE_INT ? "int" : "double",
                                           vysExp->type == TYPE_STRING ? "String" : vysExp->type == TYPE_INT ? "int" : "double");
               return SEM_ERROR;
            }
            case TYPE_INT:
               if (!top->next->next->var && !top->var)
               {
                  //val->value.intValue = top->next->next->value.intValue * top->value.intValue;
                  item = listInsert(generateInstruction(I_MUL, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                  //val->value.intValue = ((tVar*)top->next->next->var)->value.intValue * top->value.intValue; 
                  item = listInsert(generateInstruction(I_MUL_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                 // val->value.intValue = top->next->next->value.intValue * ((tVar*)top->var)->value.intValue; 
                  item = listInsert(generateInstruction(I_MUL_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                  //val->value.intValue = ((tVar*)top->next->next->var)->value.intValue * ((tVar*)top->var)->value.intValue;
                  item = listInsert(generateInstruction(I_MUL_BOTH, top->next->next->var, top->var, val)); 
               }

               debug("Vytvarim instrukci I_MUL (INT)\n");
               break;
            case TYPE_DOUBLE: 
               if (!top->next->next->var && !top->var)
               {
                  //val->value.doubleValue = top->next->next->value.doubleValue * top->value.doubleValue;
                  item = listInsert(generateInstruction(I_MUL, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                 // val->value.doubleValue = ((tVar*)top->next->next->var)->value.doubleValue * top->value.doubleValue; 
                  item = listInsert(generateInstruction(I_MUL_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                 // val->value.doubleValue = top->next->next->value.doubleValue * ((tVar*)top->var)->value.doubleValue; 
                  item = listInsert(generateInstruction(I_MUL_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                 // val->value.doubleValue = ((tVar*)top->next->next->var)->value.doubleValue * ((tVar*)top->var)->value.doubleValue;
                  item = listInsert(generateInstruction(I_MUL_BOTH, top->next->next->var, top->var, val)); 
               }         
               debug("Vytvarim instrukci I_MULV (DOUBLE)\n"); 
               
            break;
            case TYPE_STRING:
               break;
         }
         break;
      }

      case '<':
      {

         switch (controlType(oper, typ))
         {
            
            case TYPE_UNDEF:
            {
               debug("Nekompatibilni datove type %s - %s\n", typ == TYPE_STRING ? "String" : typ == TYPE_INT ? "int" : "double",
                                           vysExp->type == TYPE_STRING ? "String" : vysExp->type == TYPE_INT ? "int" : "double");
               return SEM_ERROR;
            }

            case TYPE_INT:
            case TYPE_DOUBLE:
               if (!top->next->next->var && !top->var)
               {
                  item = listInsert(generateInstruction(I_LESS, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                 // printf("%d %f %d\n", top->next->next->var->type, top->value.doubleValue, val->typ);
                  item = listInsert(generateInstruction(I_LESS_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                  item = listInsert(generateInstruction(I_LESS_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                  item = listInsert(generateInstruction(I_LESS_BOTH, top->next->next->var, top->var, val)); 
               }
            }

         break;
      }
      case '>':
      {

         switch (controlType(oper, typ))
         {
            
            case TYPE_UNDEF:
            {
               debug("Nekompatibilni datove type %s - %s\n", typ == TYPE_STRING ? "String" : typ == TYPE_INT ? "int" : "double",
                                           vysExp->type == TYPE_STRING ? "String" : vysExp->type == TYPE_INT ? "int" : "double");
               return SEM_ERROR;
            }

            case TYPE_INT:
            case TYPE_DOUBLE:
               if (!top->next->next->var && !top->var)
               {
                  item = listInsert(generateInstruction(I_GREATER, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                  item = listInsert(generateInstruction(I_GREATER_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                  item = listInsert(generateInstruction(I_GREATER_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                  item = listInsert(generateInstruction(I_GREATER_BOTH, top->next->next->var, top->var, val)); 
               }
            }

         break;
         //printf("a,sd;lkasl;dk;lsadk;alskdl;ask\n");
      }
      case '!':
      {

         switch (controlType(oper, typ))
         {
            
            case TYPE_UNDEF:
            {
               debug("Nekompatibilni datove type %s - %s\n", typ == TYPE_STRING ? "String" : typ == TYPE_INT ? "int" : "double",
                                           vysExp->type == TYPE_STRING ? "String" : vysExp->type == TYPE_INT ? "int" : "double");
               return SEM_ERROR;
            }

            case TYPE_INT:
            case TYPE_DOUBLE:
               if (!top->next->next->var && !top->var)
               {
                  item = listInsert(generateInstruction(I_NOT_EQUAL, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                  item = listInsert(generateInstruction(I_NOT_EQUAL_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                  item = listInsert(generateInstruction(I_NOT_EQUAL_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                  item = listInsert(generateInstruction(I_NOT_EQUAL_BOTH, top->next->next->var, top->var, val)); 
               }
            }

         break;
         //printf("a,sd;lkasl;dk;lsadk;alskdl;ask\n");
      }
      case 'E':
      {

         switch (controlType(oper, typ))
         {
            
            case TYPE_UNDEF:
            {
               debug("Nekompatibilni datove type %s - %s\n", typ == TYPE_STRING ? "String" : typ == TYPE_INT ? "int" : "double",
                                           vysExp->type == TYPE_STRING ? "String" : vysExp->type == TYPE_INT ? "int" : "double");
               return SEM_ERROR;
            }

            case TYPE_INT:
            case TYPE_DOUBLE:
               if (!top->next->next->var && !top->var)
               {
                  item = listInsert(generateInstruction(I_EQUAL, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                  item = listInsert(generateInstruction(I_EQUAL_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                  item = listInsert(generateInstruction(I_EQUAL_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                  item = listInsert(generateInstruction(I_EQUAL_BOTH, top->next->next->var, top->var, val)); 
               }
            }

         break;
         //printf("a,sd;lkasl;dk;lsadk;alskdl;ask\n");
      }
      case 'G':
      {

         switch (controlType(oper, typ))
         {
            
            case TYPE_UNDEF:
            {
               debug("Nekompatibilni datove type %s - %s\n", typ == TYPE_STRING ? "String" : typ == TYPE_INT ? "int" : "double",
                                           vysExp->type == TYPE_STRING ? "String" : vysExp->type == TYPE_INT ? "int" : "double");
               return SEM_ERROR;
            }

            case TYPE_INT:
            case TYPE_DOUBLE:
               if (!top->next->next->var && !top->var)
               {
                  item = listInsert(generateInstruction(I_GREATER_EQUAL, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                  item = listInsert(generateInstruction(I_GREATER_EQUAL_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                  item = listInsert(generateInstruction(I_GREATER_EQUAL_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                  item = listInsert(generateInstruction(I_GREATER_EQUAL_BOTH, top->next->next->var, top->var, val)); 
               }
            }

         break;
        // printf("a,sd;lkasl;dk;lsadk;alskdl;ask\n");
      }
      case 'L':
      {

         switch (controlType(oper, typ))
         {
            
            case TYPE_UNDEF:
            {
               debug("Nekompatibilni datove type %s - %s\n", typ == TYPE_STRING ? "String" : typ == TYPE_INT ? "int" : "double",
                                           vysExp->type == TYPE_STRING ? "String" : vysExp->type == TYPE_INT ? "int" : "double");
               return SEM_ERROR;
            }

            case TYPE_INT:
            case TYPE_DOUBLE:
               if (!top->next->next->var && !top->var)
               {
                  item = listInsert(generateInstruction(I_LESS_EQUAL, top->next->next, top, val));
               }
               else if (top->next->next->var && !top->var)
               {
                  item = listInsert(generateInstruction(I_LESS_EQUAL_FIRST, top->next->next->var, top, val));
               }
               else if (!top->next->next->var && top->var)
               {
                  item = listInsert(generateInstruction(I_LESS_EQUAL_SECOND, top->next->next, top->var, val));
               }
               else if (top->next->next->var && top->var)
               {
                  item = listInsert(generateInstruction(I_LESS_EQUAL_BOTH, top->next->next->var, top->var, val)); 
               }
            }

         break;
      }

   }

   top = val;
  // printVal();


}
void printVal()
{
   tValueStack *tmp = top;
   //printf("-----------\n");
   while(tmp)
   {
      switch (tmp->typ)
      {
         case TYPE_INT:
         {
            printf("%d\n", tmp->value.intValue);
            break;
         }
         case TYPE_DOUBLE:
         {
            printf("%f\n", tmp->value.doubleValue); 
            break;

         }
         case TYPE_STRING:
         {
            printf(" %s\n", tmp->value.stringValue); 
            break;

         }
         case PLUS:
            printf("+\n");
       
            break;

         case MINUS:
            printf("-\n");
    
            break;
         case DIVISION:
            printf("/\n");

            break;
         case MULTIPLIER:
            printf("*\n");

            break;
      }
      tmp=tmp->next;
   }

   printf("-----------\n");

}

void sumLogicVal()
{

}

void sumVal(int single)
{
   tValueStack *tmp = top;
   int i = 0;
   int typ = TYPE_UNDEF;
   char oper = 'u';
   tValueStack *tmp1 = top;
   if (!single)
   {
      while(tmp)
      {
         switch (tmp->typ)
         {

            case TYPE_INT:
            {
               if(typ == TYPE_UNDEF) {  debug("Nastavuji typ INT\n"); typ = TYPE_INT; }
               else if(typ == TYPE_INT) {}
               else if (typ == TYPE_DOUBLE) { debug("Pretypovavam INT na  DOUBLE\n"); }
               else if (typ == TYPE_STRING && oper == '+') { debug("Pretypovavam INT na  String\n"); typ = TYPE_STRING; }
               else { debug("Spatne typy\n"); exit(4); }
              // printf("INT HODNOTA %d\n", tmp->value.intValue);
               break;
            }
            case TYPE_DOUBLE:
            {
               if(typ == TYPE_UNDEF)  typ = TYPE_DOUBLE;
               else if(typ == TYPE_DOUBLE) {}
               else if (typ == TYPE_INT) { debug("Pretypovavam INT na DOUBLE\n"); typ = TYPE_DOUBLE; }
               else if (typ == TYPE_STRING && oper == '+') { debug("Pretypovavam DOUBLE na  String\n"); typ = TYPE_STRING; }
               else { debug("Spatne typy\n"); exit(4); }
               //printf("DOUBLE HODNOTA %f\n", tmp->value.doubleValue); 
               break;

            }
            case TYPE_STRING:
            {
               typ = TYPE_STRING;
               //if (oper != '+' && oper != 'u')// printf("ERRORR STRING\n");
               //printf("String HODNOTA %s\n", tmp->value.stringValue); 
               break;

            }
            case PLUS:
              // printf("+\n");
               oper = '+';
               break;

            case MINUS:
               printf("-\n");
               oper = '-';
               break;
            case DIVISION:
               //printf("/\n");
               oper = '/';
               break;
            case MULTIPLIER:
              // printf("*\n");
               oper = '*';
               break;
            case LESS:
               oper = '<';
               break;
            case GREATER:
               oper = '>';
               break;
            case NOT_EQUAL:
               oper = '!';
               break;
            case EQUAL:
               oper = 'E';
               break;
            case GREATER_OR_EQUAL:
               oper = 'G';
               break;
            case LESS_OR_EQUAL:
               oper = 'L';
               break;
         }
         i++;
         if (i == 3) 
         {
            break;
         }
         tmp=tmp->next;

      }
      if (i==3)
      {
         calcVal(oper, typ);
         //top = tmp;  // nevim co dela
      }
   }
   else
   {
      vysExp->type = top->typ;
      if (typ == TYPE_INT)
         vysExp->value.intValue = top->value.intValue;
      else if (typ == TYPE_DOUBLE)
         vysExp->value.doubleValue = top->value.doubleValue;
      else if (typ == TYPE_STRING)
         vysExp->value.stringValue = top->value.stringValue;

      //listInsert(generateInstruction(I_ASSIGN_EXPR, top, NULL, vysExp));
   }
/*
   tValueStack * typp = top;
   while(typp)
   {
      if(typp->typ == TYPE_INT)
      printf("CISSSSSSSSSSSSSlo %d\n",typp->value.intValue );
   else
      printf("neni INT\n");

   typp = typp->next;
   }*/
 //printVal();

}

int pop(bool logic)
{

   int result;
   tExpStack *tmp = zasobnik;
   int i = 0;
   int y = 0;
   int vysledek;
   while (zasobnik->data != ':')
   {
      i++;
      zasobnik = zasobnik->next;
   } 
   zasobnik = tmp;
   char pole[i+1];
   while (zasobnik->data != ':')
   {
      pole[y] = zasobnik->data;
      y++;
      tExpStack *tmp = zasobnik;
      zasobnik = zasobnik->next;
      free(tmp);
   }
   tmp = zasobnik;

   zasobnik = zasobnik->next;
   free(tmp);
   pole[y] = '\0';

   onlyOne--;
  // printf("TOKEN TYPE:%d\n",token.type);
   if      (!(vysledek = strcmp(pole,"p")))  { onlyOne++; sumVal(1); if ((result = push('E', PUSH_REDUCE, NULL))) return result; }
   else if (!(vysledek = strcmp(pole,"E+E"))) { sumVal(0); if ((result = push('E', PUSH_REDUCE, NULL))) return result; }
   else if (!(vysledek = strcmp(pole,"E-E"))) { sumVal(0); if ((result = push('E', PUSH_REDUCE, NULL))) return result; }
   else if (!(vysledek = strcmp(pole,"i")))   { sumVal(1); if ((result = push('E', PUSH_REDUCE, NULL))) return result; }
   else if (!(vysledek = strcmp(pole,"d")))   { sumVal(1); if ((result = push('E', PUSH_REDUCE, NULL))) return result; }
   else if (!(vysledek = strcmp(pole,"s")))   { sumVal(1); if ((result = push('E', PUSH_REDUCE, NULL))) return result; }
   else if (!(vysledek = strcmp(pole,"E*E"))) { sumVal(0); if ((result = push('E', PUSH_REDUCE, NULL))) return result; }
   else if (!(vysledek = strcmp(pole,"E/E"))) { sumVal(0); if ((result = push('E', PUSH_REDUCE, NULL))) return result; }
   else if (!(vysledek = strcmp(pole,"E("))&&(token.type!=SEMICOLON))  {if ((result = push('E', PUSH_REDUCE, NULL))) return result; }
   else
   {
      if (logic)
      {

         if (!(vysledek=strcmp(pole,"E<E"))) { sumVal(0); push('E', PUSH_REDUCE, NULL); }
         else if (!(vysledek=strcmp(pole,"E>E"))) { sumVal(0); push('E', PUSH_REDUCE, NULL); }
         else if (!(vysledek=strcmp(pole,"EoE"))) { sumVal(0); push('E', PUSH_REDUCE, NULL); }
         else if (!(vysledek=strcmp(pole,"EgE"))) { sumVal(0); push('E', PUSH_REDUCE, NULL); }
         else if (!(vysledek=strcmp(pole,"EeE"))) { sumVal(0); push('E', PUSH_REDUCE, NULL); }
         else if (!(vysledek=strcmp(pole,"EnE"))) { sumVal(0); push('E', PUSH_REDUCE, NULL); }
         else { return SYNTAX_ERROR; }
      }
      else 
      {
         //printf("1121\n");
         return SYNTAX_ERROR;
      }
   }     
      
  // vypis_zasobniku(zasobnik);
   
   return SUCCESS;
}

int initVysExp()
{
   vysExp = calloc(1, sizeof(tToken));
   if (!vysExp) return INTERNAL_ERROR;
   vysExp->type = TYPE_UNDEF;
   return SUCCESS;
}

int initConStr()
{
   conStr.len = 0;
   conStr.alloc = 20;
   conStr.str = malloc(20);
   if (!conStr.str) return INTERNAL_ERROR;
   conStr.concat = true;
   conStr.plus = false;
   return SUCCESS;

}

int get_int_len (int value){
  int l=1;
  while(value>9){ l++; value/=10; }
  return l;
}

int exprString()
{
   int result;
   while(token.type != SEMICOLON)
   {
      tVar *var = calloc(1, sizeof(tVar));
      if(!var) return INTERNAL_ERROR;


      switch (token.type)
      {

         case LIT_INT: 
            var->value.intValue=atoi(token.id);
            var->type = TYPE_INT;
            var->init = true;
            listInsert(generateInstruction(I_CONCAT, var, NULL, NULL));
            break;
         case LIT_DOUBLE:  ;
            char *ptr; 
            var->value.doubleValue=strtod(token.id,&ptr);
            var->type = TYPE_DOUBLE;
            var->init = true;
            listInsert(generateInstruction(I_CONCAT, var, NULL, NULL));
            break;
         case CHAIN: 
            var->value.stringValue=token.id;
            var->type = TYPE_STRING;
            var->init = true;
            listInsert(generateInstruction(I_CONCAT, var, NULL, NULL));
            break;
         case IDENTIFIER:
         {
            tList *data = lookVarStackExp();
            if (!data) { return SEM_ERROR; }
            var = data->dataPtr;
            listInsert(generateInstruction(I_CONCAT, var, NULL, NULL));
            break;
         }
         
         case FULL_IDENTIFIER:
         {
            char *className;
            char *id;
            id = parseFullId(&className, token.id);
            if (!id) return INTERNAL_ERROR;
            tList *data = tsRead(globalTS, className);
            if (!data) { debug("Nedeklarovana funkce/promenna\n"); return SEM_ERROR; }
            tClass *class = data->dataPtr;
            data = tsRead(class->symbolTable, id);
            if (!data) { debug("Nedeklarovana funkce/promenna\n"); return SEM_ERROR; }
            if (data->func) { debug("funkce ve vyrazu\n"); return SEM_ERROR; } 
            var = data->dataPtr;
            listInsert(generateInstruction(I_CONCAT, var, NULL, NULL));
            break;
         }
         default:
            debug("konkatenace fail\n");
            exit(4);
      }
     // printf("NEXT\n");
      if((result = getToken(&token))) { debug("%s\n", "ERROR - v LEX"); return result; } 
      if((token.type != PLUS) && (token.type != SEMICOLON)) { debug("konkatenace +\n"); exit(4); }
      if (token.type != SEMICOLON)
         if((result = getToken(&token))) { debug("%s\n", "ERROR - v LEX"); return result; } 
   }
   return SUCCESS;
}

int expression(bool logic, tStack *stackTop)
{   
   int result;
   if (token.type == SEMICOLON) return SYNTAX_ERROR;

   if (initVysExp()) return INTERNAL_ERROR;
   if (initConStr()) return INTERNAL_ERROR;

   stack = stackTop;
   if(retType == TYPE_STRING)
   {
      if(exprString()) return 4;
      retType = TYPE_UNDEF;
      return SUCCESS;
   }
   

   onlyOne = 0;

   if ((result = push('$', PUSH_END, NULL))) { return result; } 
   if ((result = processing_expression(logic))) { return result; }
   cisteni();

  // printf("TOTOTTOOTOOT %d %d \n", vysExp->type);

   if (vysExp->type == TYPE_INT)
   {
     // printf("VYSLEDEK typu INT %d \n",top->value.intValue);
      vysExp->value.intValue = top->value.intValue;
   }
   else if (vysExp->type == TYPE_DOUBLE)
   {
     // printf("VYSLEDEK typu DOUBLE %f\n",top->value.doubleValue);
      vysExp->value.doubleValue = top->value.doubleValue;
   }
   else if (vysExp->type == TYPE_STRING && conStr.concat)
   {
      vysExp->value.stringValue = conStr.str;
     // printf("VYSLEDEK typu STRING %s\n",vysExp->value.stringValue);
   }
   else if (!conStr.plus)
   {
      debug("Chyba typu string\n");
   }
   else
   {
     // printf("asdasd\n");
   }

  // printf("TOTOTOTOOT\n\n\n %d %d %d\n", vysExp->type, retType, conStr.concat);
   if (retType == TYPE_STRING && !conStr.concat)
   {
      debug("CHYBA STRING SPATNE HODNOTY ATD\n");
      exit(4);
   }
   if ( (retType == TYPE_INT) && (vysExp->type == TYPE_DOUBLE) )
   {
      debug("DOUBLE TO INT\n");
      exit(4);
   }


   if (onlyOne)
      listInsert(generateInstruction(I_ASSIGN_EXPR, top, NULL, vysExp));
   else
      listInsert(generateInstruction(I_ASSIGN_EXPR, top, vysExp, vysExp));


   retType = TYPE_UNDEF;
   debug("Expr: vse ok %d\n" , result);
   return result;     
}

tExpStack* nearestTerminal()
{
   tExpStack *vys = zasobnik; 
   while (vys)
   {
      switch (vys->data)
      {
         case 'p':
         case 'i':
         case 'd':
         case 's':
         case '$':
         case '+':
         case '-':
         case '*':
         case '/': 
         case '(':
         case ')':
         case '<':
         case '>':
         case '=':
         case '!':
         case 'o':
         case 'g':
         case 'e':
         case 'n':
         return vys;

         case 'r': 
            return NULL;

         default: 
            break;
      }
      vys=vys->next;
   }
   return NULL;

}

int processing_expression(bool logic)
{
   bool end = false;
   int row_index=0;
   int col_index=0;
   int result;
   tExpStack *pom;
   bool firstTime = true;
   if (NULL == (pom = nearestTerminal())) {  return SYNTAX_ERROR; } 


   for (int i = 0; (!end) && (i < VELIKOST_TABULKY); i++)
   {
      if (giveTok(&token) == PrecedencniTabulka[0][i])
      {
         col_index = i;
      }


      if (pom->data == PrecedencniTabulka[i][0])
      {
         row_index = i;
      }

      if (row_index && col_index) end = true;
      
      if (i == (VELIKOST_TABULKY-1) && !end) { return SYNTAX_ERROR; }
   }
   //printf("TOKEN: %c\n POM: %c\n",giveTok(&token),pom->data);
  //vypis_zasobniku();
   
   if ((col_index == 16) && (row_index == 17) && ('$' == zasobnik->data)) { return SYNTAX_ERROR; }
   
   switch (PrecedencniTabulka[row_index][col_index])
   {
      case '<': 
      {

         if ((result = push(':', PUSH_STOP, pom))) { return result; }
         if ((result = push(giveTok(&token), PUSH_NEW, NULL))) { return result; }
         if((result = getToken(&token))) { debug("%s\n", "ERROR - v LEX"); return result; } 

         if ((result = processing_expression(logic))) { return result; }
         break;
      }
     case '=': {
          if ((result = pop(logic))) { return result; }
          if((result = getToken(&token))) { debug("%s\n", "ERROR - v LEX"); return result; }
          
         if ((result = processing_expression(logic))) { return result; }
         
      }
      break;

      case '>': 
      {

         if ((result = pop(logic))) { return result; }
         if ((result = processing_expression(logic))) { return result; }
         break;
      }
      case '-':{
         if (logic && firstTime)
         {}
         else
          return SYNTAX_ERROR;
      }

      

      default:  
         return SUCCESS;
   }

   return SUCCESS;
}

void cisteni()
{

   while (zasobnik)
   {
      tExpStack *tmp=zasobnik;
      tmp->data="";
      zasobnik=zasobnik->next;
      free(tmp);
   }
}


void vypis_zasobniku()
{
   tExpStack *aktualni;
   aktualni=zasobnik;
   printf("Zasobnik obsahuje polozky: ");
   while (aktualni)
   {
      printf("%c ",aktualni->data);
      aktualni=aktualni->next;
   }
   printf("\n");

}