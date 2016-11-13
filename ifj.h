#ifndef IFJ_H
#define IFJ_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef enum {
	SUCCESS             = 0,
	LEX_ERROR           = 1,
	SYNTAX_ERROR        = 2,
	SEM_ERROR           = 3,
	SEM_TYPE_ERROR      = 4,
	SEM_OTHER_ERROR     = 6,
	INPUT_ERROR         = 7,
	UNINITIALIZED_ERROR = 8,
	DIV_ERROR           = 9,
	OTHER_RNT_ERROR     = 10,
	INTERNAL_ERROR      = 99,
	FILE_ERROR          = 99,
} errCode;


#endif // IFJ_H