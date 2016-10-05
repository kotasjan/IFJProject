#include "parser.h"
#include "ifj.h"
#include "ial.h"


char *keyWordsArray[]={"if","else","while","return","private","public","Main","class"};
char *reservedWords[]={"int","double","String","void"};

void initTables(){


} // Inicializuje tabulku symbolu, klicovych slov atd ... 
int parse(tList *ukazatel){
  tToken data; 
  int token = getToken(&data);



} // Funkce si vyzada dalsi token ( "get_token()" ) a nasledne zacne overovat spravnost kodu ( "program()" ).
int program(); // Funkce na zaklade ziskaneho tokenu rozhodne o co se jedna. Funkce pozna zda se jedna o identifikator ci klicove slovo a overi syntaktickou spravnost.
void clearTables(); //Odinicializuje tabulky symbolu, klicovych slov atd ...


