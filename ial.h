#ifndef IAL_H
#define IAL_H

#include "ifj.h"

/**
* \brief vrati delku retezce
* \param s retezec
* \return [INT] pocet znaku
*/
int length(char *s);

char *substr(char *s, int i, int n);
int compare(char *s1, char *s2);
int find(char *s, char *search);
char *sort(char *s);

#endif // IAL_H