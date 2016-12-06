#ifndef SCANER_H
#define SCANER_H
#include "ial.h"
#include "ifj.h"
#include <stdlib.h>
#include <stdio.h>
#define REALLOC_SIZE 20

typedef enum{

	IDENTIFIER = 0,
	FULL_IDENTIFIER,
	COMMA,					// specialni symboly od 2
	SEMICOLON,
	DOT,
	END_OF_FILE,
	LEFT_BRACKET,
	RIGHT_BRACKET,
	LEFT_SQUARE_BRACKET,
	RIGHT_SQUARE_BRACKET,
	LEFT_CURLY_BRACKET,
	RIGHT_CURLY_BRACKET,
	PLUS=100,					// aritmeticke operatory od 100
	MINUS,
	DIVISION,
	MULTIPLIER,
	INC,
	DEC,
	GREATER=200,				// logicke operatory od 200
	NOT_EQUAL,
	EQUAL,
	LESS,
	GREATER_OR_EQUAL,
	LESS_OR_EQUAL,
	ASSIGNMENT,
	EXCLAMATION,
	INT=300,					// datove typy od 300
	DOUBLE,
	STRING,
	VOID,
	BOOLEAN,
	CHAIN,    					// Je to misto stringu protoze ten uz je v enumu pod timhle.
	LIT_INT,
	LIT_DOUBLE,
	IF=400,						// klicova slova od 400
	ELSE,
	WHILE,
	FOR,
	DO,
	BREAK,
	CONTINUE,
	RETURN,
	PRIVATE,
	PUBLIC,
	MAIN,
	CLASS,
	STATIC,
	FALSE,
	TRUE,
} state;

enum{
	IS_DEFAULT = 0,		// vychozi stav KA
	IS_SIMPLE_ID,		// [a-z] [A-Z] [0-9] _ $
	IS_SIMPLE_NUMBER,	// [0-9]
	IS_FULL_ID,			// x.a
	IS_PRE_FULL_ID,		// x.
	IS_DECIMAL_NUMBER,	// [0-9].[0-9]
	IS_EXP_NUMBER,		// [0-9]eE[0-9]
	IS_PRE_DECIMAL_NUMBER,	// [0-9].
	IS_EXP_AFTER_SIGN_NUMBER,	// [0-9]eE+-[0-9]
	IS_PRE_EXP_AFTER_SIGN_NUMBER,	// [0-9]eE+-
	IS_DEC_EXP_NUMBER,	// [0-9].[0-9]eE[0-9]
	IS_DEC_EXP_AFTER_SIGN_NUMBER,	// [0-9].[0-9]eE+-[0-9]
	IS_PRE_DEC_EXP_AFTER_SIGN_NUMBER,	// [0-9].[0-9]eE+-
	IS_MULTIPLICATION,	// * */
	IS_LESS,			// < <=
	IS_GREATER,			// > >=
	IS_EXCLAMATION,		// !=   (! rozsireni)
	IS_ASSIGNMENT,		// = ==
	IS_SLASH,			// / /* //
	IS_SINGLE_COMMENT,	// //
	IS_FULL_COMMENT,	// /*
	IS_FULL_COMMENT_END,	// */
	IS_STRING_LITERAL,	// "
	IS_STRING_LIT_ESCAPE,	// " ... \" \n \\ \t
	IS_STRING_LIT_ESCAPE_OCT_1,	// \[0xx-3xx]			celkove [001-377]
	IS_STRING_LIT_ESCAPE_OCT_2, // \[x0x-x7x]
	IS_STRING_LIT_ESCAPE_OCT_3, // \[xx1-xx7]
	//IS_AND,	// rozsireni &&
	//IS_OR,	// rozsireni ||
	IS_PLUS,			// + (++ rozsireni)
	IS_MINUS,			// - (-- rozsireni)
} inner_state;


/*
enum{
	INT = 0,
	DOUBLE = 1,
	STRING = 2,
	VOID = 3,
} status;
*/

typedef struct token
{
  char *id;    // Nazev retezce
  unsigned length;      // Pocet znaku + koncova 0
  unsigned allocSize;   // Pocet alokovanych bajtu
  state type;
  union 
  {
  	int intValue;
  	double doubleValue;
  	char *stringValue;
  }value;
} tToken;

//int getToken(tToken *data); // Funkce lexikalniho analyzatoru. Vrati napriklad "IDENTIFIKATOR" nebo "END_OF_FILE".
//typedef int table;
errCode getToken(tToken *data);

void setFile(FILE *fp);
void setFileToBegin();





#endif // SCANER_H