#include <string.h>
#include <ctype.h>

#include "scaner.h"
#include "ial.h"
#include "ifj.h" 
#include "tokenfifo.h"

// To co muze token vratit

FILE *file;

void setFileToBegin()
{
   rewind(file);
}

void setFile(FILE *fp)
{
   file = fp;
}

int checkSize(tToken *data)
{
   if (data->length+1 >= data->allocSize)
   {

      while ( data->length+1 >= data->allocSize )
      {
         data->allocSize += REALLOC_SIZE;
      }
      data->id = realloc (data->id, sizeof(char) * (data->allocSize) );
      if (data->id == NULL)
      {
         return -1;
      }
   }
   return 1;
}

errCode getToken(tToken *data){           // je treba FILE * file parametr nebo globalni promenna?
   int state = IS_DEFAULT;
   int c;
   int octave = 0;

   tToken *tmp = getTokenFifo();
   if (tmp)
   {
      memcpy(data, tmp, sizeof(tToken));
      return SUCCESS;
   }



   //tToken data;          // vnitrni struktura tokenu, pri kazdem zavolani fce se vytvori tento vnitrni token kam se ukladaji nactene znaky (= jmena identifikatoru, konstatnty)
                        // pokud se najde ID tak se nactene znaky (resp. jejich ukazatel?) pouziji jako klic do hash table a vlozi se do ni novy zaznam.
                        // pokud se nacte operator nebo neco jineho tak se pomoci fce free naalokovane misto opet uvolni
                        // pokud se nacte cislo tak se ukazatel na nactene znaky vrati v void *result a ty se pak dekoduji pomoci atoi nebo strtod
                        
                        // Klic do HT by mel byt ukazatel, jednak aby se pak mohlo volat free na pamet kterou alokuju a taky a taky protoze klic musi mit jasnou velikost
   data->id = calloc(1,24);
   data->length = 0;
   data->allocSize = 24;   
   
   if (data->id == NULL)
   {
      return INTERNAL_ERROR;           // asi vratit neco jineho nez LEX_ERROR...
   }

   while (1)
   {     
      c = getc(file);
           
      switch (state)
      {
         case IS_DEFAULT:
            if (c == '(') { free(data->id); data->type = LEFT_BRACKET;              return SUCCESS; }
            else if (c == ')') { free(data->id); data->type = RIGHT_BRACKET;        return SUCCESS; }
            else if (c == '[') { free(data->id); data->type = LEFT_SQUARE_BRACKET;  return SUCCESS; }
            else if (c == ']') { free(data->id); data->type = RIGHT_SQUARE_BRACKET; return SUCCESS; }
            else if (c == '{') { free(data->id); data->type = LEFT_CURLY_BRACKET;   return SUCCESS; }
            else if (c == '}') { free(data->id); data->type = RIGHT_CURLY_BRACKET;  return SUCCESS; }
            else if (c == '.') { free(data->id); data->type = DOT;                  return LEX_ERROR; }
            else if (c == ',') { free(data->id); data->type = COMMA;                return SUCCESS; }
            else if (c == ';') { free(data->id); data->type = SEMICOLON;            return SUCCESS; }
            else if (c == '*') { free(data->id); data->type = MULTIPLIER;           return SUCCESS; }    // */ musi predchazet /*, proto pri IS_DEFAULT jedine return MULTIPLIER
            else if (c == '+') { free(data->id); data->type = PLUS;					return SUCCESS; }
            else if (c == '-') { free(data->id); data->type = MINUS;                return SUCCESS; }
			
            else if (c == '<') state = IS_LESS;
            else if (c == '>') state = IS_GREATER;
            else if (c == '!') state = IS_EXCLAMATION;
            else if (c == '=') state = IS_ASSIGNMENT;
            else if (c == '/') state = IS_SLASH;   
            else if (c == '"') state = IS_STRING_LITERAL;               
            else if (isspace(c)) state = IS_DEFAULT;
			      
            else if (isalpha(c) || c == '_' || c == '$')
            {
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
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
                  free(data->id);
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
            
               state = IS_SIMPLE_NUMBER;
            }

            else if (c == EOF) { data->type = END_OF_FILE; return SUCCESS; }
            else return LEX_ERROR;
            
         break;
         
         case IS_LESS:
            if (c == '=') { data->type = LESS_OR_EQUAL; return SUCCESS; }
            else
            {
               ungetc(c, file);
               free(data->id);
               data->type = LESS;
               return SUCCESS;
            }
         break;
         
         case IS_GREATER:
            if (c == '=') { data->type = GREATER_OR_EQUAL; return SUCCESS; }
            else
            {
               ungetc(c, file);
               free(data->id);
               data->type = GREATER;
               return SUCCESS;
            }
         break;
         
         case IS_EXCLAMATION:
            if (c == '=') { data->type = NOT_EQUAL; return SUCCESS; } 
            else
            {
               ungetc(c, file);
               free(data->id);
               return EXCLAMATION;
            }
         break;
         
         case IS_ASSIGNMENT:
            if (c == '=') { data->type = EQUAL; return SUCCESS; }
            else
            {
               ungetc(c, file);
               free(data->id);
               data->type = ASSIGNMENT; 
               return SUCCESS;
            }
         break;
         
         case IS_SLASH:
            if (c == '/') state = IS_SINGLE_COMMENT;
            else if (c == '*') state = IS_FULL_COMMENT;
            else
            {
               ungetc(c, file);
               free(data->id);
               data->type = DIVISION;
               return SUCCESS;
            }
         break;
         
         case IS_STRING_LITERAL:
            if (c == EOF) { free(data->id); return LEX_ERROR; }         // nebo END_OF_FILE?
            else if (c == '"') {  data->type = CHAIN; return SUCCESS; }  /*free(data->id);*/  // myslim ze to tu ma byt bez free
            else if (c == '\n') { free(data->id); return LEX_ERROR; }
            else if (c != '\\')
            {
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
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
         
         /*case IS_PLUS:															// SMAZAT 
            if (c == '+') { data->type = INC; return SUCCESS; }
            else
            {
               ungetc(c, file);
               free(data->id);
               data->type = PLUS;
               return SUCCESS;
            }
         break;

         case IS_MINUS:															// SMAZAT 
            if (c == '-') { data->type = DEC; return SUCCESS; }
            else
            {
               ungetc(c, file);
               free(data->id);
               data->type = MINUS;
               return SUCCESS;
            }
         break;*/
         
         case IS_SINGLE_COMMENT:
            if (c == EOF) { free(data->id); data->type = END_OF_FILE; return SUCCESS; }            
            else if (c == '\n') state = IS_DEFAULT;
            else
            {
               state = IS_SINGLE_COMMENT;
            }
         break;
         
         case IS_FULL_COMMENT:
            if (c == EOF) { free(data->id); data->type = END_OF_FILE; return LEX_ERROR; }          // LEX_ERROR viz forum
            else if (c == '*') state = IS_FULL_COMMENT_END;
            else
            {
               state = IS_FULL_COMMENT;
            }
         break;
         
         case IS_FULL_COMMENT_END:
            if (c == EOF) { free(data->id); data->type = END_OF_FILE; return LEX_ERROR; }             // LEX_ERROR viz forum
            else if (c == '/') state = IS_DEFAULT;
            else
            {
               state = IS_FULL_COMMENT;
            }
         break;
         
         case IS_STRING_LIT_ESCAPE:
            if (c == EOF) { free(data->id); return LEX_ERROR; }
            else if (c == '"' || c == 'n' || c == 't' || c == '\\') 
            {
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
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
				if (c == '0' || c == '1' || c == '2' || c == '3') 
				{
				   octave += (c-'0') * 64;
				   state = IS_STRING_LIT_ESCAPE_OCT_1;                
				}
				else
				{
				   free(data->id);
				   return LEX_ERROR;
				}
            }
            else
            {
               free(data->id);
               return LEX_ERROR;
            }
         break;
         
               
         case IS_STRING_LIT_ESCAPE_OCT_1:
            if (c == EOF) { free(data->id); return LEX_ERROR; }
            
            if (isdigit(c) && c != '9' && c != '8') 
            {
               octave += (c-'0') * 8;
               state = IS_STRING_LIT_ESCAPE_OCT_2;                
            }
            else
            {
               free(data->id);
               return LEX_ERROR;
            }
         break;

         case IS_STRING_LIT_ESCAPE_OCT_2:
            if (c == EOF) { free(data->id); return LEX_ERROR; }
            
            if (isdigit(c) && c != '9' && c != '8') 
            {
               octave += (c-'0');
               
               if (octave > 255 ) { free(data->id); return LEX_ERROR; }
               
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = octave;
               (data->length)++;
               (data->id)[data->length] = '\0';
                              
               octave = 0;
               state = IS_STRING_LITERAL;                
            }
            else
            {
               free(data->id);
               return LEX_ERROR;
            }
         break;
         
         case IS_SIMPLE_ID:
           // if (c == EOF) { free(data->id); return LEX_ERROR; }						
            if (isdigit(c) || isalpha(c) || c == '_' || c == '$')
            {
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
            
               state = IS_SIMPLE_ID;               
            }
            else if (c == '.')               // jeste se dohodnout
            {
               if (checkSize(data) == -1)
               {
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
            
               state = IS_PRE_FULL_ID;              
            }
            else
            {
               ungetc(c, file);
               
               if (strcmp(data->id, "int") == 0) { free(data->id); data->type = INT; return SUCCESS; }         
               else if (strcmp(data->id, "double") == 0) { free(data->id); data->type = DOUBLE; return SUCCESS; }
               else if (strcmp(data->id, "String") == 0) { free(data->id); data->type = STRING; return SUCCESS; }
               else if (strcmp(data->id, "void") == 0) { free(data->id); data->type = VOID; return SUCCESS; }

            
               else if (strcmp(data->id, "boolean") == 0) { free(data->id); data->type = BOOLEAN; return SUCCESS; }  // BOOLEAN x boolean?
               else if (strcmp(data->id, "break") == 0) { free(data->id); data->type = BREAK; return SUCCESS; }
               else if (strcmp(data->id, "class") == 0) { free(data->id); data->type = CLASS; return SUCCESS; }
               else if (strcmp(data->id, "continue") == 0) { free(data->id); data->type = CONTINUE; return SUCCESS; }
               else if (strcmp(data->id, "do") == 0) { free(data->id); data->type = DO; return SUCCESS; }
               else if (strcmp(data->id, "else") == 0) { free(data->id); data->type = ELSE; return SUCCESS; }
               else if (strcmp(data->id, "false") == 0) { free(data->id); data->type = FALSE; return SUCCESS; }
               else if (strcmp(data->id, "for") == 0) { free(data->id); data->type = FOR; return SUCCESS; }
               else if (strcmp(data->id, "if") == 0) { free(data->id); data->type = IF; return SUCCESS; }
               else if (strcmp(data->id, "return") == 0) { free(data->id); data->type = RETURN; return SUCCESS; }
               else if (strcmp(data->id, "static") == 0) { free(data->id); data->type = STATIC; return SUCCESS; }
               else if (strcmp(data->id, "true") == 0) { free(data->id); data->type = TRUE; return SUCCESS; }
               else if (strcmp(data->id, "while") == 0) { free(data->id); data->type = WHILE; return SUCCESS; }
               
               else
               {
                  data->type = IDENTIFIER;
                  return SUCCESS;
               } 
            }
         break;

         case IS_PRE_FULL_ID:
           // if (c == EOF) return LEX_ERROR;
            if (isalpha(c) || c == '_' || c == '$')
            {
               if (checkSize(data) == -1)
               {
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
            
               state = IS_FULL_ID;              
            }
            else
            {
               free(data->id);
               return LEX_ERROR;
            }
         break;
		 
         case IS_FULL_ID:
            //if (c == EOF) return LEX_ERROR;
            if (isdigit(c) || isalpha(c) || c == '_' || c == '$')
            {
               if (checkSize(data) == -1)
               {
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
            
               state = IS_FULL_ID;              
            }
            else
            {
               ungetc(c, file);
               data->type = FULL_IDENTIFIER;
               return SUCCESS;      // doplnit do scanner.h?
            }
         break;
         
         case IS_SIMPLE_NUMBER:
            if (isdigit(c) || c == '.' || c == 'e' || c == 'E')
            {
               
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
			   
			   if ((data->length) > 10)					// rozsah C-int -> coz znamena max ~ 2 000 000 000 (10 cislic)
			   {
                  free(data->id);
                  return LEX_ERROR;				   
			   }
               
               if (isdigit(c)) state = IS_SIMPLE_NUMBER;
               else if (c == '.') state = IS_PRE_DECIMAL_NUMBER;
               else if (c == 'e' || c == 'E') state = IS_EXP_NUMBER;
            }      
            else
            {
               ungetc(c, file);
               data->type = LIT_INT;
               return SUCCESS;            
            }
         break;
         
         case IS_PRE_DECIMAL_NUMBER:
            if (isdigit(c))
            {
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
               
               state = IS_DECIMAL_NUMBER;
            }      
            else
            {
                  free(data->id);
                  return LEX_ERROR;           
            }
         break;
		 
         case IS_DECIMAL_NUMBER:
            if (isdigit(c) || c == 'e' || c == 'E')
            {
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
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
               data->type = LIT_DOUBLE;
               return SUCCESS;            
            }
         break;
         
         case IS_EXP_NUMBER:
            if (isdigit(c) || c == '-' || c == '+')
            {
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
               
               if (isdigit(c)) state = IS_EXP_AFTER_SIGN_NUMBER;
			   else state = IS_PRE_EXP_AFTER_SIGN_NUMBER;
            }           
            else
            {
                  free(data->id);
                  return LEX_ERROR;         
            }
         break;
   
         case IS_DEC_EXP_NUMBER:
            if (isdigit(c) || c == '-' || c == '+')
            {
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
               
               if (isdigit(c)) state = IS_DEC_EXP_AFTER_SIGN_NUMBER;
			   else state = IS_PRE_DEC_EXP_AFTER_SIGN_NUMBER;
            }       
            else
            {
                  free(data->id);
                  return LEX_ERROR;          
            }
         break;
         
         case IS_PRE_DEC_EXP_AFTER_SIGN_NUMBER:
            if (isdigit(c))
            {
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
               
               state = IS_DEC_EXP_AFTER_SIGN_NUMBER;
            }  
            else
            {
                  free(data->id);
                  return LEX_ERROR;           
            }
         break;
		 
         case IS_DEC_EXP_AFTER_SIGN_NUMBER:
            if (isdigit(c))
            {
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
               
               state = IS_DEC_EXP_AFTER_SIGN_NUMBER;
            }  
            else
            {
               ungetc(c, file);
               data->type = LIT_DOUBLE;
               return SUCCESS;            
            }
         break;
         
         case IS_PRE_EXP_AFTER_SIGN_NUMBER:
            if (isdigit(c))
            {
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
               
               state = IS_EXP_AFTER_SIGN_NUMBER;
            }        
            else
            {
                  free(data->id);
                  return LEX_ERROR;        
            }
         break;
		 
         case IS_EXP_AFTER_SIGN_NUMBER:
            if (isdigit(c))
            {
               if (checkSize(data) == -1)
               {
                  free(data->id);
                  return INTERNAL_ERROR;
               }
               
               (data->id)[data->length] = c;
               (data->length)++;
               (data->id)[data->length] = '\0';
               
               state = IS_EXP_AFTER_SIGN_NUMBER;
            }        
            else
            {
               ungetc(c, file);
               data->type = LIT_DOUBLE;
               return SUCCESS;         
            }
         break;
      }
   }
}// Funkce lexikalniho analyzatoru. Vrati napriklad "IDENTIFIKATOR" nebo "END_OF_FILE".
