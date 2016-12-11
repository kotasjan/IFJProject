#include "ial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int length(char *s) {

	return strlen(s);

}

char *substr(char *s, int i, int n) {

	if( strlen(s) < (i + n) )
		return NULL;

	char *str = malloc( (n + 1) * sizeof(char) );
	if(!str)
		return NULL;

	return memcpy( str, s + i, n );

}

int compare(char *s1, char *s2) {

    int i = 0;
    while (s1[i] != '\0') {
    	
        if( s2[i] == '\0' ) return 1; 
        else if( s1[i] > s2[i] ) return 1; 
        else if( s1[i] < s2[i] ) return -1; 

        i++;
    } 
	if (s2[i] != '\0') return -1;

    return 0;

}



int readInt() {
	char * str = malloc(2048);
	if (str == NULL) return -1;
	
	fgets (str, 2048, stdin);			// cte dokud nenarazi na \0 nebo \n
	
	if (str[strlen(str)-1] == '\n')		//fgets pridava na konec \n -> to pak dela neplechu
	{
		str[strlen(str)-1] = '\0';
	}
	
	for(int i = 0; i < strlen(str); i++)
	{
		if (!isdigit(str[i])) 
		{ 
			free(str);
			exit(7);		// neodpovida zadani - pouze cislice
		}
	}
	
	int x = atoi (str);		// neni treba resit podteceni/preteceni
	free(str);
	return x;
}

double readDouble() {
	char * str = malloc(2048);
	if (str == NULL) return -1;
	
	fgets (str, 2048, stdin);
	
	if (str[strlen(str)-1] == '\n')		//fgets pridava na konec \n -> to pak dela neplechu
	{
		str[strlen(str)-1] = '\0';
	}
	
	for(int i = 0; i < strlen(str); i++)
	{
		if (isdigit(str[i]) || str[i] == 'E' || str[i] == 'e' || str[i] == '+' || str[i] == '-' || str[i] == '.')		// strtod bere i napr 'p', 'inf' atd
		{
			;
		}
		else
		{
			free(str);
			exit(7);	
		}
			
	}
	
	if (str[0] == '+' || str[0] == '-')
	{
		free(str);
		exit(7);							// strtod to bere, ale zadani to nepovoluje
	}
	
	char* error;
	
	double x = strtod (str, &error);
	if (error[0] != '\0')
	{
		free(str);
		exit(7);
	}
	free(str);
	return x;
}

String readString()
{
	char * str = malloc(8192);
	if (str == NULL) return NULL;
	
	fgets (str, 8192, stdin);			// fgets - prida na konec napr \n ?
  int i = 0;
  while (i < 8192)
  {
    if (str[i] == '\n')
    {
      str[i] = '\0';
      break;
    }
    i++;
  }
	return str;
}

void print_int(int x)
{
	char * str = malloc(24);
	if (str == NULL) return;
	
	sprintf(str, "%d", x);
	printf("%s", str);
	
	free(str);
	return;
}

void print_double(double x)
{
	char * str = malloc(64);
	if (str == NULL) return;
	
	sprintf(str, "%g", x);
	printf("%s", str);
	
	free(str);
	return;
}

void print_string(char * x)
{
	printf("%s", x);
	
	return;
}

String sort(String s)
{
	return QuickSort(s, 0, strlen(s)-1);
}

void qpartition(char * pole, int left, int right, int * i, int * j)
{
    int PM;
    int tmp;

    *i = left;
    *j = right;

    PM = pole[(*i+*j)/2];

    do
    {
        while (pole[*i] < PM) *i += 1;
        while (pole[*j] > PM) *j -= 1;

        if (*i <= *j)
        {
            tmp = pole[*i];
            pole[*i] = pole[*j];
            pole[*j] = tmp;
            *i += 1;
            *j -= 1;
        }
    }
    while (*j>*i);
}

char * QuickSort(char * pole, int left, int right)
{
    int i,j;
    qpartition(pole,left,right,&i,&j);
    if (left<j)  QuickSort(pole,left,j);
    if (i<right) QuickSort(pole,i,right);
    return pole; 
}

int find(String s, String search)
{
    char CharJump[256];
    ComputeJumps(search , CharJump);

    return BMA(search, s, CharJump);

}

int BMA (char *P, char *T, char * CharJump)
{
    int j,k;

    j = strlen(P)-1;
    k = strlen(P)-1;

    while( j < strlen(T) && k>-1)
    {
        if (T[j] == P[k])
        {
            j -= 1;
            k -= 1;
        }
        else
        {
            j = j + CharJump[T[j]];
            k = strlen(P)-1;
        }
    }

    if (k == -1)  return j+1;

    return -1;
}

void ComputeJumps(char * P, char * CharJump)
{
    int ch;
    int k;

    for(ch = 0; ch < 256; ch++)
    {
        CharJump[ch] = strlen(P);
    }

    for(k = 0; k < strlen(P); k++)
    {
       CharJump[P[k]] = strlen(P)-k-1;
    }
}



int hashFunction(const char *key, unsigned htab_size)
{
    int retval = 1;
    int keylen = strlen(key);
    for ( int i=0; i<keylen; i++ )
        retval += key[i];
    return ( retval % htab_size );
}

void tsInit(table *TS)
{

    if((*TS) == NULL)
        return ;
    for(int i = 0; i<TABLE_SIZE ;i++)
    {
        ((*TS)[i]) = NULL;
    }
    return;

}

tList* tsSearch ( table* TS, char *key ) {
    
    if((*TS) == NULL) return NULL;
    int hash = hashFunction(key, TABLE_SIZE);
    tList * tmp = (*TS)[hash];
    if(tmp)
    {
        while (tmp)
        {
            if(!(strcmp(tmp->key, key)))
                return tmp;
            else
                tmp = tmp->next;
        }
    }
    return NULL;
}

int tsInsert ( table* TS, char *key, void *data ) {

    tList *tmp = tsSearch(TS, key); 
    if(tmp) 
        return 2;
    else 
    {
        tmp = calloc(1, sizeof(tList));
        if(!tmp)
            return 1;
        tmp->key = key;
        tmp->dataPtr = data; 
        int hash = hashFunction(key, TABLE_SIZE);
        if((*TS)[hash])
        {
            tmp->next = (*TS)[hash];
        }
        else
            tmp->next = NULL;

        (*TS)[hash] = tmp;            
        return 0;
    }
}

void* tsRead ( table* TS, char *key ) {

   tList *item = tsSearch (TS, key);
   if(!item) 
      return NULL;
   else
   {
      return item;
   }
}








