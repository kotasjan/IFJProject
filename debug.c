#include "debug.h"
#include "scaner.h"
#include "ifj.h"


char *printTok(tToken *token)
{
   switch(token->type)
   {
      case IDENTIFIER:
         return token->id;
         break;
      case FULL_IDENTIFIER:
         return token->id;
         break;
      case COMMA:     
         return ",";
         break;
      case SEMICOLON:
         return ";"; 
         break;
      case DOT:
         return ".";
         break;
      case END_OF_FILE:
         return "_EoF";
         break;
      case LEFT_BRACKET:
         return "(";
         break;
      case RIGHT_BRACKET:
         return ")";
         break;
      case LEFT_SQUARE_BRACKET:
         return "[";
         break;
      case RIGHT_SQUARE_BRACKET:
         return "]";
         break;
      case LEFT_CURLY_BRACKET:
         return "{";
         break;
      case RIGHT_CURLY_BRACKET:
         return "}";
         break;
      case PLUS: 
         return "+";
         break;
      case MINUS:
         return "-";
         break;
      case DIVISION:
         return "/";
         break;
      case MULTIPLIER:
         return "*";
         break;
      case INC: 
         return "++";
         break;
      case DEC: "--";
         return ;
         break;
      case GREATER:    
         return ">";
         break;     
      case NOT_EQUAL:
         return "!=";
         break;
      case EQUAL:
         return "==";
         break;
      case LESS:
         return ;
         break;
      case GREATER_OR_EQUAL:
         return ">=";
         break;
      case LESS_OR_EQUAL:
         return "<=";
         break;
      case ASSIGNMENT:
         return "=";
         break;
      case EXCLAMATION:
         return "!";
         break;
      case INT:       
         return "int";
         break;
      case DOUBLE:
         return "double";
         break;
      case STRING:
         return token->id;
         break;
      case VOID:
         return "void";
         break;
      case BOOLEAN:
         return "boolen";
         break;
      case CHAIN:   
         return token->id;
         break;             
      case LIT_INT:
         return token->id;
         break;
      case LIT_DOUBLE:
         return token->id;
         break;
      case IF:     
         return "if";
         break;        
      case ELSE:
         return "else";
         break;
      case WHILE:
         return "while";
         break;
      case FOR:
         return "for";
         break;
      case DO:
         return "do";
         break;
      case BREAK:
         return "break";
         break;
      case CONTINUE:
         return "continue";
         break;
      case RETURN:
         return "return";
         break;
      case PRIVATE: 
         return "private";
         break;
      case PUBLIC:
         return "public";
         break;
      case MAIN:
         return "main";
         break;
      case CLASS:
         return "class";
         break;
      case STATIC:
         return "static";
         break;
      case FALSE:
         return "false";
         break;
      case TRUE:
         return "true";
         break;
   }
}