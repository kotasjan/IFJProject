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
		data->id = realloc (data->id, sizeof(char) * ((data->allocSize)+REALLOC_SIZE) );
		if (data->id == NULL)
		{
			return -1;
		}
	}
	return 1;
}

int getToken(void *result, table * TS){				// je treba FILE * file parametr nebo globalni promenna?
	int state = IS_DEFAULT;
	int c;
	int octave = 0;

	tToken data;				// vnitrni struktura tokenu, pri kazdem zavolani fce se vytvori tento vnitrni token kam se ukladaji nactene znaky (= jmena identifikatoru, konstatnty)
								// pokud se najde ID tak se nactene znaky (resp. jejich ukazatel?) pouziji jako klic do hash table a vlozi se do ni novy zaznam.
								// pokud se nacte operator nebo neco jineho tak se pomoci fce free naalokovane misto opet uvolni
								// pokud se nacte cislo tak se ukazatel na nactene znaky vrati v void *result a ty se pak dekoduji pomoci atoi nebo strtod
								
								// Klic do HT by mel byt ukazatel, jednak aby se pak mohlo volat free na pamet kterou alokuju a taky a taky protoze klic musi mit jasnou velikost
	data.id = malloc(24);
	data.length = 0;
	data.allocSize = 24;	
	
	if (data.id == NULL)
	{
		return LEX_ERROR;				// asi vratit neco jineho nez LEX_ERROR...
	}

	while (1)
	{     
		c = getc(file);
		     
		switch (state)
		{
			case IS_DEFAULT:
				if (c == '(') { free(data.id); return LEFT_BRACKET; }
				else if (c == ')') { free(data.id); return RIGHT_BRACKET; }
				else if (c == '[') { free(data.id); return LEFT_SQUARE_BRACKET; }
				else if (c == ']') { free(data.id); return RIGHT_SQUARE_BRACKET; }
				else if (c == '{') { free(data.id); return LEFT_CURLY_BRACKET; }
				else if (c == '}') { free(data.id); return RIGHT_CURLY_BRACKET; }
				else if (c == '.') { free(data.id); return DOT; }
				else if (c == ',') { free(data.id); return COMMA; }
				else if (c == ';') { free(data.id); return SEMICOLON; }
				else if (c == '*') { free(data.id); return MULTIPLIER; }		// */ musi predchazet /*, proto pri IS_DEFAULT jedine return MULTIPLIER
				
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
					if (checkSize(&data) == -1)
					{
						free(data.id);
						return LEX_ERROR;
					}
					
					(data.id)[data.length] = c;
					(data.length)++;
					(data.id)[data.length] = '\0';
					
					state = IS_SIMPLE_ID;
				}
				
				else if (isdigit(c))
				{
					if (checkSize(&data) == -1)
					{
						free(data.id);
						return LEX_ERROR;
					}
					
					(data.id)[data.length] = c;
					(data.length)++;
					(data.id)[data.length] = '\0';
				
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
					free(data.id);
					return LESS;
				}
			break;
			
			case IS_GREATER:
				if (c == '=') return GREATER_OR_EQUAL;
				else
				{
					ungetc(c, file);
					free(data.id);
					return GREATER;
				}
			break;
			
			case IS_EXCLAMATION:
				if (c == '=') return NOT_EQUAL;
				else
				{
					ungetc(c, file);
					free(data.id);
					return EXCLAMATION;
				}
			break;
			
			case IS_ASSIGNMENT:
				if (c == '=') return EQUAL;
				else
				{
					ungetc(c, file);
					free(data.id);
					return ASSIGNMENT;
				}
			break;
			
			case IS_SLASH:
				if (c == '/') state = IS_SINGLE_COMMENT;
				else if (c == '*') state = IS_FULL_COMMENT;
				else
				{
					ungetc(c, file);
					free(data.id);
					return DIVISION;
				}
			break;
			
			case IS_STRING_LITERAL:
				if (c == EOF) { free(data.id); return LEX_ERROR; }			// nebo END_OF_FILE?
				else if (c == '"') { free(data.id); return CHAIN; }
				else if (c == '\n') { free(data.id); return LEX_ERROR; }
				else if (c != '\\')
				{
					if (checkSize(&data) == -1)
					{
						free(data.id);
						return LEX_ERROR;
					}
					
					(data.id)[data.length] = c;
					(data.length)++;
					(data.id)[data.length] = '\0';
				
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
					free(data.id);
					return PLUS;
				}
			break;

			case IS_MINUS:
				if (c == '-') return DEC;
				else
				{
					ungetc(c, file);
					free(data.id);
					return MINUS;
				}
			break;
			
			case IS_SINGLE_COMMENT:
				if (c == EOF) { free(data.id); return END_OF_FILE; }				// nebo LEX_ERROR ? viz soubory z wiki
				else if (c == '\n') state = IS_DEFAULT;
				else
				{
					state = IS_SINGLE_COMMENT;
				}
			break;
			
			case IS_FULL_COMMENT:
				if (c == EOF) { free(data.id); return END_OF_FILE; }				// nebo LEX_ERROR ? viz soubory z wiki
				else if (c == '*') state = IS_FULL_COMMENT_END;
				else
				{
					state = IS_FULL_COMMENT;
				}
			break;
			
			case IS_FULL_COMMENT_END:
				if (c == EOF) { free(data.id); return END_OF_FILE; }					// nebo LEX_ERROR ? viz soubory z wiki
				else if (c == '/') state = IS_DEFAULT;
				else
				{
					state = IS_FULL_COMMENT;
				}
			break;
			
			case IS_STRING_LIT_ESCAPE:
				if (c == EOF) { free(data.id); return LEX_ERROR; }
				else if (c == '"' || c == 'n' || c == 't' || c == '\\') 
				{
					if (checkSize(&data) == -1)
					{
						free(data.id);
						return LEX_ERROR;
					}
					
					if (c == '"') (data.id)[data.length] = '"';
					else if (c == 'n') (data.id)[data.length] = '\n';
					else if (c == 't') (data.id)[data.length] = '\t';
					else if (c == '\\') (data.id)[data.length] = '\\';
					
					(data.length)++;
					(data.id)[data.length] = '\0';
				
					state = IS_STRING_LITERAL;
				}
				else if (isdigit(c))
				{
					ungetc(c, file);
					state = IS_STRING_LIT_ESCAPE_OCT_1;
				}
				else
				{
					free(data.id);
					return LEX_ERROR;
				}
			break;
			
			
			case IS_STRING_LIT_ESCAPE_OCT_1:
				if (c == EOF) { free(data.id); return LEX_ERROR; }

				if (c == '0' || c == '1' || c == '2' || c == '3') 
				{
					octave += c * 64;
					state = IS_STRING_LIT_ESCAPE_OCT_2;						
				}
				else
				{
					free(data.id);
					return LEX_ERROR;
				}
			break;
					
			case IS_STRING_LIT_ESCAPE_OCT_2:
				if (c == EOF) { free(data.id); return LEX_ERROR; }
				
				if (isdigit(c) && c != '9' && c != '8') 
				{
					octave += c * 8;
					state = IS_STRING_LIT_ESCAPE_OCT_3;						
				}
				else
				{
					free(data.id);
					return LEX_ERROR;
				}
			break;

			case IS_STRING_LIT_ESCAPE_OCT_3:
				if (c == EOF) { free(data.id); return LEX_ERROR; }
				
				if (isdigit(c) && c != '9' && c != '8') 
				{
					octave += c;
					
					if (octave > 256 ) { free(data.id); return LEX_ERROR; }
					
					if (checkSize(&data) == -1)
					{
						free(data.id);
						return LEX_ERROR;
					}
					
					(data.id)[data.length] = octave;
					(data.length)++;
					(data.id)[data.length] = '\0';
										
					octave = 0;
					state = IS_STRING_LITERAL;						
				}
				else
				{
					free(data.id);
					return LEX_ERROR;
				}
			break;
			
			case IS_SIMPLE_ID:
				if (c == EOF) { free(data.id); return LEX_ERROR; }
				else if (isdigit(c) || isalpha(c) || c == '_' || c == '$')
				{
					if (checkSize(&data) == -1)
					{
						free(data.id);
						return LEX_ERROR;
					}
					
					(data.id)[data.length] = c;
					(data.length)++;
					(data.id)[data.length] = '\0';
				
					state = IS_SIMPLE_ID;					
				}
				else
				{
					ungetc(c, file);
					
					if (strcmp(data.id, "int") == 0) { free(data.id); return INT; }			
					else if (strcmp(data.id, "double") == 0) { free(data.id); return DOUBLE; }
					else if (strcmp(data.id, "String") == 0) { free(data.id); return STRING; }
					else if (strcmp(data.id, "void") == 0) { free(data.id); return VOID; }

					
					else if (strcmp(data.id, "boolean") == 0) { free(data.id); return BOOLEAN; }	// BOOLEAN x boolean?
					else if (strcmp(data.id, "break") == 0) { free(data.id); return BREAK; }
					else if (strcmp(data.id, "class") == 0) { free(data.id); return CLASS; }
					else if (strcmp(data.id, "continue") == 0) { free(data.id); return CONTINUE; }
					else if (strcmp(data.id, "do") == 0) { free(data.id); return DO; }
					else if (strcmp(data.id, "else") == 0) { free(data.id); return ELSE; }
					else if (strcmp(data.id, "false") == 0) { free(data.id); return FALSE; }
					else if (strcmp(data.id, "for") == 0) { free(data.id); return FOR; }
					else if (strcmp(data.id, "if") == 0) { free(data.id); return IF; }
					else if (strcmp(data.id, "return") == 0) { free(data.id); return RETURN; }
					else if (strcmp(data.id, "static") == 0) { free(data.id); return STATIC; }
					else if (strcmp(data.id, "true") == 0) { free(data.id); return TRUE; }
					else if (strcmp(data.id, "while") == 0) { free(data.id); return WHILE; }
					
					else
					{
						if (hash_table_is_this_key_already_in_hash_table(TS, data.id) == 1) result = hash_table_get_pointer_to_data(TS, data.id);
						else result = hash_table_insert_key_and_return_pointer_to_data(TS, data.id);		// nebo vrati pointer na data v parametru,...
						return IDENTIFIER;
					} 
				}
			break;
			
			case IS_SIMPLE_NUMBER:
				if (isdigit(c) || c == '.' || c == 'e' || c == 'E')
				{
					if (checkSize(&data) == -1)
					{
						free(data.id);
						return LEX_ERROR;
					}
					
					(data.id)[data.length] = c;
					(data.length)++;
					(data.id)[data.length] = '\0';
					
					if (isdigit(c)) state = IS_SIMPLE_NUMBER;
					else if (c == '.') state = IS_DECIMAL_NUMBER;
					else if (c == 'e' || c == 'E') state = IS_EXP_NUMBER;
				}			
				else
				{
					ungetc(c, file);
					result = (void *) data.id;		// a pak v SA pouzit funkci atoi nebo podobne...
					return INT;				
				}
			break;
			
			case IS_DECIMAL_NUMBER:
				if (isdigit(c) || c == 'e' || c == 'E')
				{
					if (checkSize(&data) == -1)
					{
						free(data.id);
						return LEX_ERROR;
					}
					
					(data.id)[data.length] = c;
					(data.length)++;
					(data.id)[data.length] = '\0';
					
					if (isdigit(c)) state = IS_DECIMAL_NUMBER;
					else if (c == 'e' || c == 'E') state = IS_DEC_EXP_NUMBER;
				}			
				else
				{
					ungetc(c, file);
					result = (void *) data.id;		// a pak v SA pouzit funkci strtod, podporuje parsovani eE+- ...
					return DOUBLE;				
				}
			break;
			
			case IS_EXP_NUMBER:
				if (isdigit(c) || c == '-' || c == '+')
				{
					if (checkSize(&data) == -1)
					{
						free(data.id);
						return LEX_ERROR;
					}
					
					(data.id)[data.length] = c;
					(data.length)++;
					(data.id)[data.length] = '\0';
					
					state = IS_EXP_AFTER_SIGN_NUMBER;
				}			
				else
				{
					ungetc(c, file);
					result = (void *) data.id;		// a pak v SA pouzit funkci strtod, podporuje parsovani eE+- ...
					return DOUBLE;				
				}
			break;
	
			case IS_DEC_EXP_NUMBER:
				if (isdigit(c) || c == '-' || c == '+')
				{
					if (checkSize(&data) == -1)
					{
						free(data.id);
						return LEX_ERROR;
					}
					
					(data.id)[data.length] = c;
					(data.length)++;
					(data.id)[data.length] = '\0';
					
					state = IS_DEC_EXP_AFTER_SIGN_NUMBER;
				}			
				else
				{
					ungetc(c, file);
					result = (void *) data.id;		// a pak v SA pouzit funkci strtod, podporuje parsovani eE+- ...
					return DOUBLE;				
				}
			break;
			
			case IS_DEC_EXP_AFTER_SIGN_NUMBER:
				if (isdigit(c))
				{
					if (checkSize(&data) == -1)
					{
						free(data.id);
						return LEX_ERROR;
					}
					
					(data.id)[data.length] = c;
					(data.length)++;
					(data.id)[data.length] = '\0';
					
					state = IS_DEC_EXP_AFTER_SIGN_NUMBER;
				}			
				else
				{
					ungetc(c, file);
					result = (void *) data.id;		// a pak v SA pouzit funkci strtod, podporuje parsovani eE+- ...
					return DOUBLE;				
				}
			break;
			
			case IS_EXP_AFTER_SIGN_NUMBER:
				if (isdigit(c))
				{
					if (checkSize(&data) == -1)
					{
						free(data.id);
						return LEX_ERROR;
					}
					
					(data.id)[data.length] = c;
					(data.length)++;
					(data.id)[data.length] = '\0';
					
					state = IS_EXP_AFTER_SIGN_NUMBER;
				}			
				else
				{
					ungetc(c, file);
					result = (void *) data.id;		// a pak v SA pouzit funkci strtod, podporuje parsovani eE+- ...
					return DOUBLE;			
				}
			break;
		}
	}
}// Funkce lexikalniho analyzatoru. Vrati napriklad "IDENTIFIKATOR" nebo "END_OF_FILE".
