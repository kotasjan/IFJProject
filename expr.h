#pragma once
#include "ial.h"

#define VELIKOST_TABULKY 18

static const char PrecedencniTabulka[VELIKOST_TABULKY][VELIKOST_TABULKY]={

/*           */{'Q','+','-','*','/','<','>','o','g','e','n','i','d','s','p','(',')','$'},
/*	   +     */{'+','<','<','<','<','>','>','>','>','>','>','<','<','<','<','<','>','>'},
/*	   -     */{'-','<','<','<','<','>','>','>','>','>','>','<','<','<','<','<','>','>'},
/*	   *     */{'*','>','>','>','>','>','>','>','>','>','>','<','<','<','<','<','>','>'},
/*	   /     */{'/','>','>','>','>','>','>','>','>','>','>','<','<','<','<','<','>','>'},
/*	   <     */{'<','<','<','<','<','-','-','-','-','-','-','<','<','<','<','<','>','>'},
/*	   >     */{'>','<','<','<','<','-','-','-','-','-','-','<','<','<','<','<','>','>'},
/*	   <=    */{'o','<','<','<','<','-','-','-','-','-','-','<','<','<','<','<','<','>'},
/*	   >=    */{'g','<','<','<','<','-','-','-','-','-','-','<','<','<','<','<','<','>'},
/*	   ==    */{'e','<','<','<','<','-','-','-','-','-','-','<','<','<','<','<','<','>'},
/*	   !=    */{'n','<','<','<','<','-','-','-','-','-','-','<','<','<','<','<','<','>'},
/*	   int   */{'i','>','>','>','>','>','>','-','-','-','-','-','-','=','-','-','>','>'},
/*    double */{'d','>','>','>','>','>','>','-','-','-','-','-','-','=','-','-','>','>'},
/*    string */{'s','>','>','>','>','>','>','-','-','-','-','-','-','=','-','-','>','>'},
/*	   i     */{'p','>','>','>','>','>','>','>','>','>','>','-','-','-','>','<','>','>'},
/*	   (     */{'(','<','<','<','<','<','<','<','<','<','<','<','<','<','<','<','=','>'},
/*	   )     */{')','>','>','>','>','>','>','>','>','>','>','-','<','-','>','>','>','>'},
/*	   $     */{'$','<','<','<','<','<','<','<','<','<','<','<','<','<','<','<','>','P'},
};
typedef struct expStack
{
   char data;
   struct expStack *next;
} tExpStack;