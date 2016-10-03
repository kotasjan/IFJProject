#ifndef SCANER_H
#define SCANER_H
#include "ifj.h"

typedef struct
{
  char * chars;		// Nazev retezce
  int length;		// Pocet znaku + koncova 0
  int all_size;		// Pocet alokovanych bajtu
} token;

#endif // SCANER_H