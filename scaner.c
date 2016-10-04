#include "scaner.h"
#include "ifj.h"
#include "ial.h"


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
	STRING,
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


int getToken(tToken *data){}// Funkce lexikalniho analyzatoru. Vrati napriklad "IDENTIFIKATOR" nebo "END_OF_FILE".
