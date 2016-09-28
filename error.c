#include "error.h"


int errorFunction(int error){

	switch(error){
		case LEX_ERROR: 
			fprintf(stderr,"Error in lexical analysis.\n"); 
			break;
		case SYNTAX_ERROR: 
			fprintf(stderr,"Error in program's syntax.\n"); 
			break;
		case SEM_ERROR: 
			fprintf(stderr,"Undefined reference.\n"); 
			break;
		case SEM_TYPE_ERROR: 
			fprintf(stderr,"Semanticka chyba typove kompatibility.\n"); 
			break;	
		case SEM_OTHER_ERROR: 
			fprintf(stderr,"Ostatni semanticke chyby.\n"); 
			break;
		case INPUT_ERROR: 
			fprintf(stderr,"Error while loading value from input.\n"); 
			break;
		case UNINITIALIZED_ERROR: 
			fprintf(stderr,"Error because of uninitialized value.\n"); 
			break;	
		case DIV_ERROR: 
			fprintf(stderr,"Error because of dividing by zero.\n"); 
			break;	
		case OTHER_RNT_ERROR: 
			fprintf(stderr,"Other runtime errors.\n"); 
			break;
		case INTERNAL_ERROR: 
			fprintf(stderr,"Internal error.\n"); 
			break;			
	}

}