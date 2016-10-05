#ifndef SCANER_H
#define SCANER_H


typedef struct
{
  char *chars;		// Nazev retezce
  unsigned length;		// Pocet znaku + koncova 0
  unsigned allocSize;	// Pocet alokovanych bajtu
} tToken;

int getToken(tToken *data); // Funkce lexikalniho analyzatoru. Vrati napriklad "IDENTIFIKATOR" nebo "END_OF_FILE".

#endif // SCANER_H