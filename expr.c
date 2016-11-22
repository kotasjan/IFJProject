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
		tmp->next=zasobnik;
		zasobnik=tmp;
      return SUCCESS;
	}
}

int push(int znacka, tExpStack *pom)
{
   tExpStack *tmp = (tExpStack*) malloc(sizeof(tExpStack));
   if (!tmp) { return INTERNAL_ERROR; }
   if (znacka == 0)
   {
      printf("AAAAAAAAAAAAAAAAAAAAAAA%c\n",giveTok(&token));
		tmp->data=giveTok(&token);
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
         printf("Ted by se to hodilo \n\n");
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
   if 	  (!(vysledek = strcmp(pole,"p"))) 	 push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"E+E"))) push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"E-E"))) push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"i"))) 	 push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"d"))) 	 push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"s"))) 	 push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"E*E"))) push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"E/E"))) push(2, zasobnik);
   else if (!(vysledek = strcmp(pole,"E(")))  push(2, zasobnik);
   else
   {
      if (podminka)
      {
         if 	  (!(vysledek=strcmp(pole,"<E")))  push(0, zasobnik);
         else if (!(vysledek=strcmp(pole,">E")))  push(0, zasobnik);
         else if (!(vysledek=strcmp(pole,"=>E"))) push(0, zasobnik);
         else if (!(vysledek=strcmp(pole,"<=E"))) push(0, zasobnik);
         else if (!(vysledek=strcmp(pole,"==E"))) push(0, zasobnik);
         else if (!(vysledek=strcmp(pole,"!=E"))) push(0, zasobnik);
         else if (!(vysledek=strcmp(pole,"E<E"))) push(2, zasobnik);
         else if (!(vysledek=strcmp(pole,"E>E"))) push(2, zasobnik);
         else if (!(vysledek=strcmp(pole,"EoE"))) push(2, zasobnik);
         else if (!(vysledek=strcmp(pole,"EgE"))) push(2, zasobnik);
         else if (!(vysledek=strcmp(pole,"EeE"))) push(2, zasobnik);
         else if (!(vysledek=strcmp(pole,"EnE"))) push(2, zasobnik);
         else return SYNTAX_ERROR;
      }
      else return SYNTAX_ERROR;
   }     
      
   vypis_zasobniku();
   
   return SUCCESS;
}

int expression(bool logic, tStack *stack1)
{
   if (logic) podminka=true;
   int result;
   if (token.type == SEMICOLON) { return SYNTAX_ERROR; }
   stack = stack1;
   tList *data;

  
   printf("ZACATEK\n\n\n");
   if ((result = stackInit())) { return result; }
   if ((result = co_delat())) { return result; }
   cisteni();
   printf("KONEC\n\n\n");
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
  printf("Neco je spatne!");

}

int co_delat(){
   bool konec=false;
   int vysledek1=0;
   int vysledek2=0;
   int result;
   tExpStack *pom;
   if (NULL == (pom = nejblizsi_terminal())) { return SYNTAX_ERROR; }
   vypis_zasobniku();
   printf("POM JE:%c\n",pom->data);
   printf("TOKEN JE: %c %d \n",giveTok(&token),token.type);
   vypis_zasobniku();
   for(int i=0;((konec==false)&&(i<VELIKOST_TABULKY));i++){
      //printf("Hledam ... %d.Pruchod\n",i);
      if(giveTok(&token)==PrecedencniTabulka[0][i]){
         printf("Nasel jsem shodu pro token na %d.prvku\n",i);
         vysledek2=i;
      }
      if(pom->data==PrecedencniTabulka[i][0]){
         vysledek1=i;
         printf("Nasel jsem shodu pro zasobnik na %d.prvku\n",i);
      }
      if((vysledek1!=0)&&(vysledek2!=0)){
         konec=true;
      }
   }
   if((vysledek2==16)&&(vysledek1==17)) return SUCCESS;
   switch(PrecedencniTabulka[vysledek1][vysledek2]){
      case '<': 
         if ((result = push(1,pom))) { return result; }
         vypis_zasobniku();
         if ((result = push(0,pom))) { return result; }
            if((result = getToken(&token))) {
               debug("%s\n", "ERROR - v LEX"); 
               return result; 
            }
            tList *data;

            if(token.type == IDENTIFIER)
            {
            	data = tsRead(stack->table, token.id);
            
         	if (data) printf("ok\n");
         	else printf("nee\n");
        		}
         vypis_zasobniku();
         if ((result = co_delat())) { return result; }


      break;
      case '>': 
                printf("Skocilo to do >\n");
                vypis_zasobniku();
                if ((result = pop())) { return result; }
                if ((result = co_delat())) { return result; }
      break;
      case '=': printf("Skocilo to do =\n");
                vypis_zasobniku();
                if ((result = pop())) { return result; }

                  if((result = getToken(&token))) {
                     debug("%s\n", "ERROR - v LEX"); 
                     return result; 
                  }

		            if(token.type == IDENTIFIER)
		            {
		            	data = tsRead(stack->table, token.id);
		            
		         	if (data) printf("ok\n");
		         	else printf("nee\n");
		        		}
               
                vypis_zasobniku();
                if ((result = co_delat())) { return result; }
      break;
      case '-': printf("Skocilo to do -\n");
                vypis_zasobniku();
                printf("%c\n",giveTok(&token));
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



void vypis_zasobniku(){
   tExpStack *aktualni;
   aktualni=zasobnik;
   if(aktualni!=NULL){
	  printf("Zasobnik obsahuje polozky: ");
	  do{
		 printf("%c ",aktualni->data);
		 aktualni=aktualni->next;
	    }while(aktualni!=NULL);
	printf("\n");
}

}