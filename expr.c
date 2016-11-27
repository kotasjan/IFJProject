#include "ial.h"
#include "scaner.h"
#include "expr.h"
#include "debug.h"
#include "ifj.h"
#include "parse_new.h"
#include <stdbool.h>

tExpStack *zasobnik;
tToken token;
bool podminka=false;
tStack *stack;
tValueStack *top;
tToken *vysExp;

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
         return SUCCESS;

      case LIT_DOUBLE:  
         if (addConCat()) return INTERNAL_ERROR;
         (*val)->value.doubleValue=token.value.doubleValue; 
         debug("Double cislo %f\n", (*val)->value.doubleValue);
         (*val)->typ = TYPE_DOUBLE;
         return SUCCESS;

      case CHAIN: 
         if (addConCat()) return INTERNAL_ERROR;
         (*val)->value.stringValue=token.value.stringValue; 
         debug("String %s\n", (*val)->value.stringValue);
         (*val)->typ = TYPE_STRING;
         return SUCCESS;

      case IDENTIFIER:
      {
         tList *data = lookVarStackExp();
         if (!data) { return SYNTAX_ERROR; }
         tVar *var = data->dataPtr;
         if (!var->init) { debug("Neinicializovana promenna ve vyrazu\n"); return SEM_ERROR; } // asi jina chyba
         return SUCCESS;
      }
      case MINUS:
      case MULTIPLIER:
      case DIVISION:
         conStr.concat = false;
         (*val)->typ = token.type;
         return SUCCESS;
      case PLUS:
         conStr.plus = true;
         (*val)->typ = token.type;
         return SUCCESS;
   }

   return 1;
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
         if(getVal(&val))
            free(val);
         else
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
   }
}

int calcVal(char oper, tType typ)
{
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
            case TYPE_INT: top->next->next->value.intValue = top->next->next->value.intValue - top->value.intValue; break;
            case TYPE_DOUBLE: top->next->next->value.doubleValue = top->next->next->value.doubleValue - top->value.doubleValue; break;
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
            case TYPE_INT:  top->next->next->value.intValue = top->value.intValue + top->next->next->value.intValue; break;
            case TYPE_DOUBLE: top->next->next->value.doubleValue = top->value.doubleValue + top->next->next->value.doubleValue; break;
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
            case TYPE_INT: top->next->next->value.intValue =  top->next->next->value.intValue / top->value.intValue; break;
            case TYPE_DOUBLE: top->next->next->value.doubleValue = top->next->next->value.doubleValue / top->value.doubleValue; break;
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
            case TYPE_INT: top->next->next->value.intValue = top->value.intValue * top->next->next->value.intValue; break;
            case TYPE_DOUBLE: top->next->next->value.doubleValue = top->value.doubleValue * top->next->next->value.doubleValue; break;
            case TYPE_STRING:
               break;
         }
         break;
      }
   }


   top = top->next->next;
  // printVal();


}
void printVal()
{
   tValueStack *tmp = top;
   printf("-----------\n");
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

void sumVal()
{
   tValueStack *tmp = top;
   int i = 0;
   int typ = TYPE_UNDEF;
   char oper = 'u';
   while(tmp)
   {
      switch (tmp->typ)
      {
         case TYPE_INT:
         {
            if(typ == TYPE_UNDEF) { debug("Nastavuji typ INT\n"); typ = TYPE_INT; }
            else if(typ == TYPE_INT) {}
            else if (typ == TYPE_DOUBLE) { debug("Pretypovavam INT na  DOUBLE\n"); tmp->value.doubleValue = tmp->value.intValue; typ = TYPE_DOUBLE; }
            else if (typ == TYPE_STRING && oper == '+') { debug("Pretypovavam INT na  String\n"); typ = TYPE_STRING; }
            else debug("Spatne typy\n");
            //printf("INT HODNOTA %d\n", tmp->value.intValue);
            break;
         }
         case TYPE_DOUBLE:
         {
            if(typ == TYPE_UNDEF)  typ = TYPE_DOUBLE;
            else if(typ == TYPE_DOUBLE) {}
            else if (typ == TYPE_INT) { debug("Pretypovavam INT na DOUBLE\n"); top->value.doubleValue = top->value.intValue; typ = TYPE_DOUBLE; }
            else if (typ == TYPE_STRING && oper == '+') { debug("Pretypovavam DOUBLE na  String\n"); typ = TYPE_STRING; }
            else debug("Spatne typy\n");
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
            //printf("+\n");
            oper = '+';
            break;

         case MINUS:
            //printf("-\n");
            oper = '-';
            break;
         case DIVISION:
            //printf("/\n");
            oper = '/';
            break;
         case MULTIPLIER:
            //printf("*\n");
            oper = '*';
            break;
      }
      i++;
      if (i == 3) break;
      tmp=tmp->next;

   }
   if (i==3)
   {
      calcVal(oper, typ);
      top = tmp;
   }
   
 //printVal();

}

int pop()
{
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

   
   if      (!(vysledek = strcmp(pole,"p")))   push('E', PUSH_REDUCE, NULL);
   else if (!(vysledek = strcmp(pole,"E+E"))) { sumVal(); push('E', PUSH_REDUCE, NULL); }
   else if (!(vysledek = strcmp(pole,"E-E"))) { sumVal(); push('E', PUSH_REDUCE, NULL); }
   else if (!(vysledek = strcmp(pole,"i")))   push('E', PUSH_REDUCE, NULL);
   else if (!(vysledek = strcmp(pole,"d")))   push('E', PUSH_REDUCE, NULL);
   else if (!(vysledek = strcmp(pole,"s")))   push('E', PUSH_REDUCE, NULL);
   else if (!(vysledek = strcmp(pole,"E*E"))) { sumVal(); push('E', PUSH_REDUCE, NULL); }
   else if (!(vysledek = strcmp(pole,"E/E"))) { sumVal(); push('E', PUSH_REDUCE, NULL); }
   else if (!(vysledek = strcmp(pole,"E(")))  {  push('E', PUSH_REDUCE, NULL); }
   else
   {
      if (podminka)
      {
         if (!(vysledek=strcmp(pole,"E<E"))) push('E', PUSH_REDUCE, NULL);
         else if (!(vysledek=strcmp(pole,"E>E"))) push('E', PUSH_REDUCE, NULL);
         else if (!(vysledek=strcmp(pole,"EoE"))) push('E', PUSH_REDUCE, NULL);
         else if (!(vysledek=strcmp(pole,"EgE"))) push('E', PUSH_REDUCE, NULL);
         else if (!(vysledek=strcmp(pole,"EeE"))) push('E', PUSH_REDUCE, NULL);
         else if (!(vysledek=strcmp(pole,"EnE"))) push('E', PUSH_REDUCE, NULL);
         else return SYNTAX_ERROR;
      }
      else return SYNTAX_ERROR;
   }     
      
   //vypis_zasobniku(zasobnik);
   
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



int expression(bool logic, tStack *stackTop)
{   
   int result;
   if (token.type == SEMICOLON) return SYNTAX_ERROR;

   if (initVysExp()) return INTERNAL_ERROR;
   if (initConStr()) return INTERNAL_ERROR;
   
   stack = stackTop;

   if ((result = push('$', PUSH_END, NULL))) { return result; } 
   if ((result = processing_expression(logic))) { return result; }
   cisteni();

/*
   if (vysExp->type == TYPE_INT)
      printf("VYSLEDEK typu INT %d \n",top->value.intValue);
   if (vysExp->type == TYPE_DOUBLE)
      printf("VYSLEDEK typu DOUBLE %f\n",top->value.doubleValue);
   if (vysExp->type == TYPE_STRING && conStr.concat)
   {
      vysExp->value.stringValue = conStr.str;
      printf("VYSLEDEK typu STRING %s\n",vysExp->value.stringValue);
   }
   else
      debug("Chyba typu string\n");
      */
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

   if (NULL == (pom = nearestTerminal())) return SYNTAX_ERROR; 


   for (int i = 0; (!end) && (i < VELIKOST_TABULKY); i++)
   {
      if (giveTok(&token) == PrecedencniTabulka[0][i])
         col_index = i;

      if (pom->data == PrecedencniTabulka[i][0])
         row_index = i;

      if (row_index && col_index) end = true;
      
      if (i == (VELIKOST_TABULKY-1) && !end) return SYNTAX_ERROR;
   }
   
   if ((col_index == 16) && (row_index == 17) && ('$' == zasobnik->data)) return SYNTAX_ERROR;
   
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
      case '=': 
         if((result = getToken(&token))) { debug("%s\n", "ERROR - v LEX"); return result; }
      case '>': 
      {
         if ((result = pop())) { return result; }
         if ((result = processing_expression(logic))) { return result; }
         break;
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