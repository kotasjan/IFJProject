#include "parser.h"
#include "ifj.h"
#include "ial.h"
#include "scaner.h"

const unsigned int numberOfKW = 8;
const unsigned int numberOfRW = 4;

char *keyWordsArray[]={"if","else","while","return","private","public","Main","class"};
char *reservedWordsArray[]={"int","double","String","void"};
tList *gSymbolTable;			

tList *initTables(){
	tList *table = malloc(sizeof(tList));		//Zaalokuje se i struktura tToken.
	if(table==NULL){
		// TODO - ERROR PRI ALOKACI PAMETI ------ SISO MAM ZAVOLAT ERROR.C A TAM NAPSAT CHYBU A NEBO TADY ROVNOU JU VYPRINTIT ? KDYZTAK TO UDELEJ :D.
	}



} 
int reservedWordsCmp(tToken *data){
int result;

for(int i=0;i!=(numberOfRW-2);i++){
		if((result = strcmp(reservedWordsArray[i],data->id))==0){
			return i;
		}
		
	}
return -1;
}

int keyWordsCmp(tToken *data){		//Projde klicova slova a rezervovana slova a pokud najde shodu vraci true
	int result;
	for(int i=0;i<(numberOfKW-2);i++){
		if((result = strcmp(keyWordsArray[i],data->id))==0){
			return i;
		}
		
	}
	
	return -1;

}

int parse(tList *ukazatel){
  tToken data; 
  int token = getToken(&data);			// Naplni se struktura o tokenu.
  gSymbolTable = initTables();

  switch(token){
  	case IDENTIFIER: 
  	{
  		int res;
  		if((res = keyWordsCmp(&data)) != -1)
  		{

  		}
  		printf("%d\n", res);
  		if((res = reservedWordsCmp(&data)) != -1)
  		{
  			ukazatel->typ = res;

  		}	//JEDNA SE O IDENTIFIKATOR ----> BUD KLICOVE SLOVO NEBO RESERVOVANE SLOVO A NEBO OPRAVDU IDENTIFIKATOR
  		printf("%d\n", res);
  					
  		break;
  	}
  	default:	break;		

  }




} 



