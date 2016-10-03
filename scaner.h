#ifndef SCANER_H
#define SCANER_H
#include "ifj.h"

typedef struct
{
  char * chars;		// Nazev retezce
  unsigned length;		// Pocet znaku + koncova 0
  unsigned alloc_size;	// Pocet alokovanych bajtu
} token;

#endif // SCANER_H