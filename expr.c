#include "ial.h"
#include "scaner.h"
#include "expr.h"
#include "debug.h"
#include "ifj.h"
#include "parse_new.h"

struct expStack *zasobnik;
tToken token;


int stackInit(){
	struct expStack *tmp=(struct expStack*)malloc(sizeof(struct expStack));
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

int push(int znacka,struct expStack *pom){
	struct expStack *tmp=(struct expStack*)malloc(sizeof(struct expStack));
   if(tmp==NULL){
		return INTERNAL_ERROR;
	}
	if(znacka==0){
		 tmp->data=giveTok(&token);
		 tmp->next=zasobnik;
		 zasobnik=tmp;
	}
   
   if(znacka==1){
      tmp->data='<';
      tmp->next=pom;
      if(pom==zasobnik){
         zasobnik=tmp;
      }
      else{
       //  printf("Ted by se to hodilo \n\n");

         zasobnik->next=tmp;
      }
      
   }

   if(znacka==2){
      tmp->data='E';
      tmp->next=zasobnik;
      zasobnik=tmp;
   }
   return SUCCESS;
}


int pop(){
   struct expStack *tmp=zasobnik;
   int i=0;
   int y=0;
   int vysledek;
   while(zasobnik->data!='<'){
      i++;
      zasobnik=zasobnik->next;
   } 
   zasobnik=tmp;
   char pole[i+1];
   while(zasobnik->data!='<'){
      pole[y]=zasobnik->data;
      y++;
      struct expStack *tmp=zasobnik;
      zasobnik=zasobnik->next;
      free(tmp);
   }
   tmp=zasobnik;
   zasobnik=zasobnik->next;
   free(tmp);
   pole[y]='\0';
   if ((vysledek=strcmp(pole,"p"))==0) push(2,zasobnik);
   else if ((vysledek=strcmp(pole,"E+E"))==0) push(2,zasobnik);
   else if ((vysledek=strcmp(pole,"E-E"))==0) push(2,zasobnik);
   else if ((vysledek=strcmp(pole,"i"))==0) push(2,zasobnik);
   else if ((vysledek=strcmp(pole,"d"))==0) push(2,zasobnik);
   else if ((vysledek=strcmp(pole,"s"))==0) push(2,zasobnik);
   else if ((vysledek=strcmp(pole,"E*E"))==0) push(2,zasobnik);
   else if ((vysledek=strcmp(pole,"E/E"))==0) push(2,zasobnik);
   else if ((vysledek=strcmp(pole,"E("))==0) push(2,zasobnik);
   else{
//printf("BOHUZEL \n"); return SYNTAX_ERROR;
   }     
   vypis_zasobniku();
   
   return SUCCESS;
}
int boolexpression(){
      int result;
}

int expression(){

      int result;
      if (token.type == SEMICOLON) { return SYNTAX_ERROR; }
     // printf("ZACATEK\n\n\n");
      if ((result = stackInit())) { return result; }
      if ((result = co_delat())) { return result; }
      cisteni();
     // printf("KONEC\n\n\n");
      return result;
         
}

struct expStack* nejblizsi_terminal(){
   struct expStack *vys=zasobnik; 
   while(vys!=NULL){
      switch(vys->data){
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
         
         return vys;
         break;
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
   struct expStack *pom;
   pom= nejblizsi_terminal();
  // printf("POM JE:%c\n",pom->data);
  // printf("TOKEN JE: %c\n",giveTok(&token));
   vypis_zasobniku();
   for(int i=0;((konec==false)&&(i<VELIKOST_TABULKY));i++){
      //printf("Hledam ... %d.Pruchod\n",i);
      if(giveTok(&token)==PrecedencniTabulka[0][i]){
      //   printf("Nasel jsem shodu pro token na %d.prvku\n",i);
         vysledek2=i;
      }
      if(pom->data==PrecedencniTabulka[i][0]){
         vysledek1=i;
      //   printf("Nasel jsem shodu pro zasobnik na %d.prvku\n",i);
      }
      if((vysledek1!=0)&&(vysledek2!=0)){
         konec=true;
      }
   }
   switch(PrecedencniTabulka[vysledek1][vysledek2]){
      case '<': 
         if ((result = push(1,pom))) { return result; }
         vypis_zasobniku();
         if ((result = push(0,pom))) { return result; }
            if((result = getToken(&token))) {
               debug("%s\n", "ERROR - v LEX"); 
               return result; 
            }
         
         vypis_zasobniku();
         if ((result = co_delat())) { return result; }


      break;
      case '>': 
           //     printf("Skocilo to do >\n");
                vypis_zasobniku();
                if ((result = pop())) { return result; }
                if ((result = co_delat())) { return result; }
      break;
      case '=': //printf("Skocilo to do =\n");
                vypis_zasobniku();
                if ((result = pop())) { return result; }

                  if((result = getToken(&token))) {
                     debug("%s\n", "ERROR - v LEX"); 
                     return result; 
                  }
               
                vypis_zasobniku();
                if ((result = co_delat())) { return result; }
      break;
      case '-':// printf("Skocilo to do -\n");
                vypis_zasobniku();
                printf("%c\n",giveTok(&token));
      break;
      default:   return SUCCESS;
   }

   
}

void cisteni(){

while(zasobnik!=NULL){
   struct expStack *tmp=zasobnik;
   tmp->data="";
   zasobnik=zasobnik->next;
   free(tmp);
}
}

int pocetPrvkuNaZasobniku(){
   struct expStack *aktualni;
   aktualni=zasobnik;
   int pocitadlo=0;
	  while(aktualni!=NULL){
		  pocitadlo++;
		  aktualni=aktualni->next;
	  }
   return pocitadlo;
}



void vypis_zasobniku(){
   struct expStack *aktualni;
   aktualni=zasobnik;
   if(aktualni!=NULL){
	  //printf("Zasobnik obsahuje polozky: ");
	  do{
		// printf("%c ",aktualni->data);
		 aktualni=aktualni->next;
	    }while(aktualni!=NULL);
	//printf("\n");
}

}