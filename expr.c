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

int stackInit(){
	tExpStack *tmp=(tExpStack*)malloc(sizeof(tExpStack));
	if(tmp==NULL){
		return INTERNAL_ERROR;
	}
	else{
		tmp->data='$';
		tmp->next=NULL;
		zasobnik=tmp;
      return SUCCESS;
	}
}
void dat_typ(tExpStack **tmp){
   (*tmp)->typ = token.type;

   switch(token.type){
      case LIT_INT: 
         (*tmp)->value.intValue=token.value.intValue; 
         //debug("Int cislo %d\n", (*tmp)->value.intValue);
         break;
      case LIT_DOUBLE:  
         (*tmp)->value.doubleValue=token.value.doubleValue; 
        // debug("Double cislo %f\n", (*tmp)->value.doubleValue);
         break;
      case CHAIN: 
         (*tmp)->value.stringValue=token.value.stringValue; 
         //debug("String %s\n", (*tmp)->value.stringValue);
         break;
      }
   return;
}

int push(int znacka, tExpStack *pom)
{
   tExpStack *tmp = (tExpStack*) malloc(sizeof(tExpStack));
   if (!tmp) { return INTERNAL_ERROR; }
   if (znacka == 0)
   {
      //printf("AAAAAAAAAAAAAAAAAAAAAAA%c\n",giveTok(&token));
		tmp->data=giveTok(&token);
      dat_typ(&tmp);
		tmp->next=zasobnik;
		zasobnik=tmp;
	}
   if (znacka == 1)
   {
      tmp->data=':';
      tmp->next=pom;
      if (pom == zasobnik)
      {
         zasobnik=tmp;
      }
      else
      {
         //printf("Ted by se to hodilo \n\n");
         zasobnik->next=tmp;
      }  
   }

   if (znacka == 2)
   {
      tmp->data='E';
      tmp->next=zasobnik;
      zasobnik=tmp;
   }   
   return SUCCESS;
}
void printValue(tExpStack *tmp)
{
   static int i = 0;
      switch (tmp->typ)
      {
         case LIT_DOUBLE:
            debug("%f\n", tmp->value.doubleValue);
            break;
         case LIT_INT:
            debug("%d\n", tmp->value.intValue);
            break;
         case CHAIN:
            debug("%s\n", tmp->value.stringValue);
            break;
         case PLUS:
            debug("+\n");
         case MULTIPLIER:
            debug("*\n");
            break;
         default:
         break;
      }
   tmp = tmp->next;
         switch (tmp->typ)
      {
         case LIT_DOUBLE:
            debug("%f\n", tmp->value.doubleValue);
            break;
         case LIT_INT:
            debug("%d\n", tmp->value.intValue);
            break;
         case CHAIN:
            debug("%s\n", tmp->value.stringValue);
            break;
         case PLUS:
            debug("+\n");
            break;
            case MULTIPLIER:
            debug("*\n");
            break;
         default:
         break;
      }
         tmp = tmp->next;
         switch (tmp->typ)
      {
         case LIT_DOUBLE:
            debug("%f\n", tmp->value.doubleValue);
            break;
         case LIT_INT:
            debug("%d\n", tmp->value.intValue);
            break;
         case CHAIN:
            debug("%s\n", tmp->value.stringValue);
            break;
         case PLUS:
            debug("+\n");
            break;
         case MULTIPLIER:
            debug("*\n");
            break;
         default:
         break;
      }


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
   /*
   if ((zasobnik->data == 'p') ||
       (zasobnik->data == 'i') || 
       (zasobnik->data == 'd') ||
       (zasobnik->data == 's'))
   {
      printValue(tmp);
   }
*/
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
  // free(tmp);
   pole[y] = '\0';

   
   if 	  (!(vysledek = strcmp(pole,"p"))) 	 push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"E+E"))) { push(2, zasobnik);}
   else if (!(vysledek = strcmp(pole,"E-E"))) push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"i"))) 	 push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"d"))) 	 push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"s"))) 	 push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"E*E"))) { push(2, zasobnik);}
   else if (!(vysledek = strcmp(pole,"E/E"))) push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"E(")))  push(2, zasobnik);
   else
   {
      if (podminka)
      {
         if (!(vysledek=strcmp(pole,"E<E"))) push(2, zasobnik);
         else if (!(vysledek=strcmp(pole,"E>E"))) push(2, zasobnik);
         else if (!(vysledek=strcmp(pole,"EoE"))) push(2, zasobnik);
         else if (!(vysledek=strcmp(pole,"EgE"))) push(2, zasobnik);
         else if (!(vysledek=strcmp(pole,"EeE"))) push(2, zasobnik);
         else if (!(vysledek=strcmp(pole,"EnE"))) push(2, zasobnik);
         else return SYNTAX_ERROR;
      }
      else return SYNTAX_ERROR;
   }     
      
   vypis_zasobniku(zasobnik);
   
   return SUCCESS;
}

int expression(bool logic, tStack *stackTop)
{
   if (logic) podminka=true;
   int result;
   if (token.type == SEMICOLON) { return SYNTAX_ERROR; }
   stack = stackTop;
   tList *data;

  
 //  printf("ZACATEK\n\n\n");
   if ((result = stackInit())) { return result; }
   if ((result = co_delat())) { return result; }
   cisteni();
   debug("Expr: vse ok %d\n" , result);
  // printf("KONEC\n\n\n");
   return result;     
}

tExpStack* nejblizsi_terminal()
{
   tExpStack *vys=zasobnik; 
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

         case 'r': return NULL;

         default: break;
      }
      vys=vys->next;
   }
 // printf("Neco je spatne!");

}

int co_delat(){
   bool konec=false;
   int vysledek1=0;
   int vysledek2=0;
   int result;
   tExpStack *pom;
   if (NULL == (pom = nejblizsi_terminal())) { return SYNTAX_ERROR; }

  // printf("POM JE:%c\n",pom->data);
  // printf("TOKEN JE: %c %d \n",giveTok(&token),token.type);
   vypis_zasobniku(zasobnik);
   for(int i=0;((konec==false)&&(i<VELIKOST_TABULKY));i++){
      //printf("Hledam ... %d.Pruchod\n",i);
      if(giveTok(&token)==PrecedencniTabulka[0][i]){
        // printf("Nasel jsem shodu pro token na %d.prvku\n",i);
         vysledek2=i;
      }
      if(pom->data==PrecedencniTabulka[i][0]){
         vysledek1=i;
        // printf("Nasel jsem shodu pro zasobnik na %d.prvku\n",i);
      }
      if((vysledek1!=0)&&(vysledek2!=0)){
         konec=true;
      }
      if(i==(VELIKOST_TABULKY-1)&&(konec==false)) return SYNTAX_ERROR;
   }
   
  // printf("%c\n",zasobnik->data );
   if((vysledek2==16)&&(vysledek1==17)&&('$'==zasobnik->data)) return SYNTAX_ERROR;
   switch(PrecedencniTabulka[vysledek1][vysledek2]){
      case '<': 
         if ((result = push(1,pom))) { return result; }
         vypis_zasobniku(zasobnik);
         if ((result = push(0,pom))) { return result; }
            if((result = getToken(&token))) {
               debug("%s\n", "ERROR - v LEX"); 
               return result; 
            }
            
        		
         vypis_zasobniku(zasobnik);
         if ((result = co_delat())) { return result; }


      break;
      case '>': 
                //printf("Skocilo to do >\n");
                vypis_zasobniku(zasobnik);
                if ((result = pop())) { return result; }
                if ((result = co_delat())) { return result; }
      break;
      case '=': //printf("Skocilo to do =\n");
                vypis_zasobniku(zasobnik);
                if ((result = pop())) { return result; }

                  if((result = getToken(&token))) {
                     debug("%s\n", "ERROR - v LEX"); 
                     return result; 
                  }

		            
               
                vypis_zasobniku(zasobnik);
                if ((result = co_delat())) { return result; }
      break;
      case '-': //printf("Skocilo to do -\n");
                vypis_zasobniku(zasobnik);
                //printf("%c\n",giveTok(&token));
      break;
      default:   return SUCCESS;
   }

   
}

void cisteni(){

while(zasobnik!=NULL){
   tExpStack *tmp=zasobnik;
   tmp->data="";
   zasobnik=zasobnik->next;
   free(tmp);
}
}

int pocetPrvkuNaZasobniku(){
   tExpStack *aktualni;
   aktualni=zasobnik;
   int pocitadlo=0;
	  while(aktualni!=NULL){
		  pocitadlo++;
		  aktualni=aktualni->next;
	  }
   return pocitadlo;
}



void vypis_zasobniku(tExpStack *zasobnik){
   tExpStack *aktualni;
   aktualni=zasobnik;
	 // printf("Zasobnik obsahuje polozky: ");
	while(aktualni){
		//printf("%c ",aktualni->data);
		aktualni=aktualni->next;
	}
     
//printf("\n");




}