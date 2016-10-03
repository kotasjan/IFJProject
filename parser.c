#include "parser.h"

void init_tables(); // Inicializuje tabulku symbolu, klicovych slov atd ... 
int analysis(tabulka_symbolu *ukazatel); // Funkce si vyzada dalsi token ( "get_token()" ) a nasledne zacne overovat spravnost kodu ( "program()" ).
int program(); // Funkce na zaklade ziskaneho tokenu rozhodne o co se jedna. Funkce pozna zda se jedna o identifikator ci klicove slovo a overi syntaktickou spravnost.
void clear_tables(); //Odinicializuje tabulky symbolu, klicovych slov atd ...


