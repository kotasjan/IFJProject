typedef struct expStack
{
   char data;
   struct expStack *next;
} tExpStack;






char giveTok(tToken *token){
switch(token->type)
   {
      case FULL_IDENTIFIER:
      case IDENTIFIER:
         return 'p';
      case STRING:
      case CHAIN:   
         return 's';
      case INT:           
      case LIT_INT:
         return 'i';
      case DOUBLE:
      case LIT_DOUBLE:
         return 'd';
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
         


}
}





const char PrecedencniTabulka[Velikost_tabulky][Velikost_tabulky]={

/*          */{'Q','+','-','*','/','<','>','o','g','e','n','i','d','s','p','(',')','$'},
/*	   +     */{'+','>','>','<','<','>','>','>','>','>','>','<','<','<','>','>','>','>'},
/*	   -     */{'-','>','>','<','<','>','>','>','>','>','>','-','<','<','>','>','>','>'},
/*	   *     */{'*','>','>','>','>','>','>','>','>','>','>','-','<','<','>','>','>','>'},
/*	   /     */{'/','>','>','>','>','>','>','>','>','>','>','-','<','<','>','>','>','>'},
/*	   <     */{'<','<','<','<','<','-','-','-','-','-','-','-','<','<','>','>','>','>'},
/*	   >     */{'>','<','<','<','<','-','-','-','-','-','-','-','<','<','>','>','>','>'},
/*	   <=    */{'o','<','<','<','<','-','-','-','-','-','-','-','<','<','>','>','>','>'},
/*	   >=    */{'g','<','<','<','<','-','-','-','-','-','-','-','<','<','>','>','>','>'},
/*	   ==    */{'e','<','<','<','<','-','-','-','-','-','-','-','<','<','>','>','>','>'},
/*	   !=    */{'n','<','<','<','<','-','-','-','-','-','-','-','<','<','>','>','>','>'},
/*	   int   */{'i','-','-','-','-','-','-','-','-','-','-','-','-','=','-','-','>','>'},
/*    double*/{'d','-','-','-','-','-','-','-','-','-','-','-','-','=','-','-','>','>'},
/*    string*/{'s','-','-','-','-','-','-','-','-','-','-','-','-','=','-','-','>','>'},
/*	   i     */{'p','>','>','>','>','>','>','>','>','>','>','-','-','-','>','>','>','>'},
/*	   (     */{'(','<','<','<','<','<','<','<','<','<','<','<','<','<','=','-','>','>'},
/*	   )     */{')','>','>','>','>','>','>','>','>','>','>','-','<','-','>','>','>','>'},
/*	   $     */{'$','<','<','<','<','<','<','<','<','<','<','<','<','<','-','P','>','>'},
};


void stackInit(){
	struct expStack *tmp=(struct expStack*)malloc(sizeof(struct expStack));
	if(tmp==NULL){
		return INTERNAL_ERROR;
	}
	else{
		tmp->data='$';
		tmp->next=zasobnik;
		zasobnik=tmp;

	}
}

void push(int znacka){
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
      tmp->next=zasobnik;
      zasobnik=tmp;
   }

}


void pop(){
	struct expStack *tmp=zasobnik;
	zasobnik=zasobnik->next;
	free(tmp);
}
int boolexpression(){
      int result;
}
int expression(){
		int result;
   		stackInit();
         co_delat();
   		
         
}

void co_delat(){
   bool konec=false;
   int vysledek1=0;
   int vysledek2=0;
   int result;
   for(int i=0;((konec==false)&&(i<Velikost_tabulky));i++){
      //printf("Hledam ... %d.Pruchod\n",i);
      if(giveTok(&token)==PrecedencniTabulka[0][i]){
         printf("Nasel jsem shodu pro token na %d.prvku\n",i);
         vysledek2=i;
      }
      if(zasobnik->data==PrecedencniTabulka[i][0]){
         vysledek1=i;
         printf("Nasel jsem shodu pro zasobnik na %d.prvku\n",i);
      }
      if((vysledek1!=0)&&(vysledek2!=0)){
         konec=true;
      }
   }
   switch(PrecedencniTabulka[vysledek1][vysledek2]){
      case '<': 
         push(1); 
         if((result = getToken(&token)) != SUCCESS) {
            debug("%s\n", "ERROR - v LEX"); 
            return result; 
         }
         push(0);

      break;
      case '>':
      break;
      case '=':
      break;
      case '-':
      break;
      default: printf("Chyba nekde v pici\n");

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
	printf("Zasobnik obsahuje polozky: ");
	do{
		printf("%c ",aktualni->data);
		aktualni=aktualni->next;
	}while(aktualni!=NULL);
	printf("\n");
}

}