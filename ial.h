#ifndef IAL_H
#define IAL_H
#include "scaner.h"

typedef struct tList{
	int typ;
	tToken token;
	struct tList *next;
}tList;

typedef struct {
	unsigned long size;
	unsigned int (*hashFunction)(const char *, unsigned);
	unsigned long n;
	tList **data;
}hTable;

unsigned int hashFunction(const char *str, unsigned htab_size);

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