#ifndef SCANER_H
#define SCANER_H

typedef enum{

	LEX_ERROR = 0,
	END_OF_FILE,
	IDENTIFIER,
	COMMA,
	SEMICOLON,
	LEFT_BRACKET,
	RIGHT_BRACKET,
	LEFT_SQUARE_BRACKET,
	RIGHT_SQUARE_BRACKET,
	LEFT_CURLY_BRACKET,
	RIGHT_CURLY_BRACKET,
	PLUS,
	MINUS,
	DIVISION,
	MULTIPLIER,
	CHAIN,    // Je to misto stringu protoze ten uz je v enumu pod timhle.
	INC,
	DEC,
	GREATER,
	NOT_EQUAL,
	EQUAL,
	LESS,
	GREATER_OR_EQUAL,
	LESS_OR_EQUAL,
	ASSIGNMENT,
	EXCLAMATION,
	DOT,
} state;

enum{
	INT = 0,
	DOUBLE = 1,
	STRING = 2,
	VOID = 3,

} status;

typedef struct
{
  char *id;		// Nazev retezce
  unsigned length;		// Pocet znaku + koncova 0
  unsigned allocSize;	// Pocet alokovanych bajtu
} tToken;

int getToken(tToken *data); // Funkce lexikalniho analyzatoru. Vrati napriklad "IDENTIFIKATOR" nebo "END_OF_FILE".

#endif // SCANER_H