//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <ctype.h>

#include "scaner.h"
#include "ifj.h"
#include "ial.h"

// To co muze token vratit

int checkSize(tToken *data)
{
	if (data->length >= data->allocSize)
	{
		data->id = realloc (data->id, sizeof(char) * ((data->allocSize)*2) );
		if (data->id == NULL)
		{
			return -1;
		}
	}
	return 1;
}

int getToken(void *result, table * TS){				// je treba FILE * file -> parametr nebo globalni promenna?
	int state = IS_DEFAULT;
	int c;
	int octave = 0;
	int octave_cnt = 0;
	int post_number = 0;


	while (1)
	{     
		c = getc(file);
		     
		switch (state)
		{
			case IS_DEFAULT:
				if (c == '(') return LEFT_BRACKET;
				else if (c == ')') return RIGHT_BRACKET;
				else if (c == '[') return LEFT_SQUARE_BRACKET;
				else if (c == ']') return RIGHT_SQUARE_BRACKET;
				else if (c == '{') return LEFT_CURLY_BRACKET;
				else if (c == '}') return RIGHT_CURLY_BRACKET;
				else if (c == '.') return DOT;
				else if (c == ',') return COMMA;
				else if (c == ';') return SEMICOLON;
				else if (c == '*') return MULTIPLIER;		// */ musi predchazet /*, proto pri IS_DEFAULT jedine return MULTIPLIER
				
				else if (c == '<') state = IS_LESS;
				else if (c == '>') state = IS_GREATER;
				else if (c == '!') state = IS_EXCLAMATION;
				else if (c == '=') state = IS_ASSIGNMENT;
				else if (c == '/') state = IS_SLASH;	
				else if (c == '"') state = IS_STRING_LITERAL;					
				else if (c == '+') state = IS_PLUS;		
				else if (c == '-') state = IS_MINUS;
				else if (isspace(c)) state = IS_DEFAULT;

				else if (isalpha(c) || c == '_' || c == '$')
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					(data->id)[data->length] = c;
					(data->length)++;
					(data->id)[data->length] = '\0';
					
					state = IS_SIMPLE_ID;
				}
				
				else if (isdigit(c))
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					(data->id)[data->length] = c;
					(data->length)++;
					(data->id)[data->length] = '\0';
				
					state = IS_SIMPLE_NUMBER;
				}

				else if (c == EOF) return END_OF_FILE;
				else return LEX_ERROR;
				
			break;
			
			case IS_LESS:
				if (c == '=') return LESS_OR_EQUAL;
				else
				{
					ungetc(c, file);
					return LESS;
				}
			break;
			
			case IS_GREATER:
				if (c == '=') return GREATER_OR_EQUAL;
				else
				{
					ungetc(c, file);
					return GREATER;
				}
			break;
			
			case IS_EXCLAMATION:
				if (c == '=') return NOT_EQUAL;
				else
				{
					ungetc(c, file);
					return EXCLAMATION;
				}
			break;
			
			case IS_ASSIGNMENT:
				if (c == '=') return EQUAL;
				else
				{
					ungetc(c, file);
					return ASSIGNMENT;
				}
			break;
			
			case IS_SLASH:
				if (c == '/') state = IS_SINGLE_COMMENT;
				else if (c == '*') state = IS_FULL_COMMENT;
				else
				{
					ungetc(c, file);
					return DIVISION;
				}
			break;
			
			case IS_STRING_LITERAL:
				if (c == EOF) return LEX_ERROR;			// nebo END_OF_FILE?
				else if (c == '"') return CHAIN;
				else if (c == '\n') return LEX_ERROR;
				else if (c != '\\')
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					(data->id)[data->length] = c;
					(data->length)++;
					(data->id)[data->length] = '\0';
				
					state = IS_STRING_LITERAL;					
				}
				else
				{
					state = IS_STRING_LIT_ESCAPE;					
				}
			break;
			
			case IS_PLUS:
				if (c == '+') return INC;
				else
				{
					ungetc(c, file);
					return PLUS;
				}
			break;

			case IS_MINUS:
				if (c == '-') return DEC;
				else
				{
					ungetc(c, file);
					return MINUS;
				}
			break;
			
			case IS_SINGLE_COMMENT:
				if (c == EOF) return END_OF_FILE;					// nebo LEX_ERROR ? viz soubory z wiki
				else if (c == '\n') state = IS_DEFAULT;
				else
				{
					state = IS_SINGLE_COMMENT;
				}
			break;
			
			case IS_FULL_COMMENT:
				if (c == EOF) return END_OF_FILE;					// nebo LEX_ERROR ? viz soubory z wiki
				else if (c == '*') state = IS_FULL_COMMENT_END;
				else
				{
					state = IS_FULL_COMMENT;
				}
			break;
			
			case IS_FULL_COMMENT_END:
				if (c == EOF) return END_OF_FILE;					// nebo LEX_ERROR ? viz soubory z wiki
				else if (c == '/') state = IS_DEFAULT;
				else
				{
					state = IS_FULL_COMMENT;
				}
			break;
			
			case IS_STRING_LIT_ESCAPE:
				if (c == EOF) return LEX_ERROR;
				else if (c == '"' || c == 'n' || c == 't' || c == '\\') 
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					if (c == '"') (data->id)[data->length] = '"';
					else if (c == 'n') (data->id)[data->length] = '\n';
					else if (c == 't') (data->id)[data->length] = '\t';
					else if (c == '\\') (data->id)[data->length] = '\\';
					
					(data->length)++;
					(data->id)[data->length] = '\0';
				
					state = IS_STRING_LITERAL;
				}
				else if (isdigit(c))
				{
					ungetc(c, file);
					state = IS_STRING_LIT_ESCAPE_OCT;
				}
				else
				{
					return LEX_ERROR;
				}
			break;
			
			
			case IS_STRING_LIT_ESCAPE_OCT:
				if (c == EOF) return LEX_ERROR;
				
				if (octave_cnt == 0)
				{
					if (c == '0' || c == '1' || c == '2' || c == '3') 
					{
						octave += c * 64;
						octave_cnt++;
						state = IS_STRING_LIT_ESCAPE_OCT;						
					}
					else
					{
						return LEX_ERROR;
					}
				}
				
				else if (octave_cnt == 1)
				{
					if (isdigit(c) && c != '9' && c != '8') 
					{
						octave += c * 8;
						octave_cnt++;
						state = IS_STRING_LIT_ESCAPE_OCT;						
					}
					else
					{
						return LEX_ERROR;
					}
				}
				
				else if (octave_cnt == 2)
				{
					if (isdigit(c) && c != '9' && c != '8') 
					{
						octave += c;
						
						if (octave > 256 ) return LEX_ERROR;
						
						if (checkSize(data) == -1)
						{
							return LEX_ERROR;
						}
						
						(data->id)[data->length] = octave;
						(data->length)++;
						(data->id)[data->length] = '\0';
											
						octave_cnt = 0;
						octave = 0;
						state = IS_STRING_LITERAL;						
					}
					else
					{
						return LEX_ERROR;
					}
				}
				
				else
				{
					return LEX_ERROR;
				}
			break;
			
			case IS_SIMPLE_ID:
				if (c == EOF) return LEX_ERROR;
				else if (isdigit(c) || isalpha(c) || c == '_' || c == '$')
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					(data->id)[data->length] = c;
					(data->length)++;
					(data->id)[data->length] = '\0';
				
					state = IS_SIMPLE_ID;					
				}
				else if (c == '.')					// jeste se dohodnout
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					(data->id)[data->length] = c;
					(data->length)++;
					(data->id)[data->length] = '\0';
				
					state = IS_FULL_ID;					
				}
				else
				{
					ungetc(c, file);
					
					if (strcmp(data->id, "int") == 0) return INT;				
					else if (strcmp(data->id, "double") == 0) return DOUBLE;
					else if (strcmp(data->id, "String") == 0) return STRING;
					else if (strcmp(data->id, "void") == 0) return VOID;

					
					else if (strcmp(data->id, "boolean") == 0) return DATA_TYPE;	// doplnit do scanner.h
					else if (strcmp(data->id, "break") == 0) return DATA_TYPE;
					else if (strcmp(data->id, "class") == 0) return DATA_TYPE;
					else if (strcmp(data->id, "continue") == 0) return DATA_TYPE;
					else if (strcmp(data->id, "do") == 0) return DATA_TYPE;
					else if (strcmp(data->id, "else") == 0) return DATA_TYPE;
					else if (strcmp(data->id, "false") == 0) return DATA_TYPE;
					else if (strcmp(data->id, "for") == 0) return DATA_TYPE;
					else if (strcmp(data->id, "if") == 0) return DATA_TYPE;
					else if (strcmp(data->id, "return") == 0) return DATA_TYPE;
					else if (strcmp(data->id, "static") == 0) return DATA_TYPE;
					else if (strcmp(data->id, "true") == 0) return DATA_TYPE;
					else if (strcmp(data->id, "while") == 0) return DATA_TYPE;
					
					else return IDENTIFIER;
				}
			break;
			
			case IS_FULL_ID:
				if (c == EOF) return LEX_ERROR;
				else if (isdigit(c) || isalpha(c) || c == '_' || c == '$')
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					(data->id)[data->length] = c;
					(data->length)++;
					(data->id)[data->length] = '\0';
				
					state = IS_FULL_ID;					
				}
				else
				{
					ungetc(c, file);
					return FULL_IDENTIFIER;		// doplnit do scanner.h?
				}
			break;
			
			case IS_SIMPLE_NUMBER:
				if (isdigit(c) || c == '.' || c == 'e' || c == 'E')
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					(data->id)[data->length] = c;
					(data->length)++;
					(data->id)[data->length] = '\0';
					
					if (isdigit(c)) state = IS_SIMPLE_NUMBER;
					else if (c == '.') state = IS_DECIMAL_NUMBER;
					else if (c == 'e' || c == 'E') state = IS_EXP_NUMBER;
				}			
				else
				{
					ungetc(c, file);
					return NUMBER;				// doplnit do scanner.h?
				}
			break;
			
			case IS_DECIMAL_NUMBER:
				if (isdigit(c) || c == 'e' || c == 'E')
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					(data->id)[data->length] = c;
					(data->length)++;
					(data->id)[data->length] = '\0';
					
					if (isdigit(c)) state = IS_DECIMAL_NUMBER;
					else if (c == 'e' || c == 'E') state = IS_DEC_EXP_NUMBER;
				}			
				else
				{
					ungetc(c, file);
					return NUMBER_DEC;				// doplnit do scanner.h?
				}
			break;
			
			case IS_EXP_NUMBER:
				if (isdigit(c) || c == '-' || c == '+')
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					(data->id)[data->length] = c;
					(data->length)++;
					(data->id)[data->length] = '\0';
					
					if (isdigit(c)) state = IS_EXP_NUMBER;
					else if (c == '-' || c == '+') state = IS_EXP_SIGN_NUMBER;
				}			
				else
				{
					ungetc(c, file);
					return NUMBER_EXP;				// doplnit do scanner.h?
				}
			break;
	
			case IS_DEC_EXP_NUMBER:
				if (isdigit(c) || c == '-' || c == '+')
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					(data->id)[data->length] = c;
					(data->length)++;
					(data->id)[data->length] = '\0';
					
					if (isdigit(c)) state = IS_DEC_EXP_NUMBER;
					else if (c == '-' || c == '+') state = IS_DEC_EXP_SIGN_NUMBER;
				}			
				else
				{
					ungetc(c, file);
					return NUMBER_DEC_EXP;				// doplnit do scanner.h?
				}
			break;
			
			case IS_DEC_EXP_SIGN_NUMBER:
				if (isdigit(c))
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					(data->id)[data->length] = c;
					(data->length)++;
					(data->id)[data->length] = '\0';
					
					state = IS_DEC_EXP_SIGN_NUMBER;
				}			
				else
				{
					ungetc(c, file);
					return NUMBER_DEC_EXP_SIGN;				// doplnit do scanner.h?
				}
			break;
			
			case IS_EXP_SIGN_NUMBER:
				if (isdigit(c))
				{
					if (checkSize(data) == -1)
					{
						return LEX_ERROR;
					}
					
					(data->id)[data->length] = c;
					(data->length)++;
					(data->id)[data->length] = '\0';
					
					state = IS_EXP_SIGN_NUMBER;
				}			
				else
				{
					ungetc(c, file);
					return NUMBER_EXP_SIGN;				// doplnit do scanner.h?
				}
			break;
		}
	}
}// Funkce lexikalniho analyzatoru. Vrati napriklad "IDENTIFIKATOR" nebo "END_OF_FILE".
