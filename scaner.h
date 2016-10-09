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
	IS_DEFAULT = 0,		// vychozi stav KA
	IS_SIMPLE_ID,		// [a-z] [A-Z] [0-9] _ $
	IS_FULL_ID,			// simple_id.simple_id
	IS_SIMPLE_NUMBER,	// [0-9]
	IS_DECIMAL_NUMBER,	// [0-9].[0-9]
	IS_EXP_NUMBER,		// [0-9]eE[0-9]
	IS_EXP_SIGN_NUMBER,	// [0-9]eE+-[0-9]
	IS_DEC_EXP_NUMBER,	// [0-9].[0-9]eE[0-9]
	IS_DEC_EXP_SIGN_NUMBER,	// [0-9].[0-9]eE+-[0-9]
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
	IS_STRING_LIT_ESCAPE_OCT,	// \[001-377]
	//IS_AND,	// rozsireni &&
	//IS_OR,	// rozsireni ||
	IS_PLUS,			// + (++ rozsireni)
	IS_MINUS,			// - (-- rozsireni)
} inner_state;


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