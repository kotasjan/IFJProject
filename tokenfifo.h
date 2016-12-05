#pragma once 

#include "ifj.h"
#include "scaner.h"


typedef struct tokenFifo
{
   tToken token;
   struct tokenFifo *next;
} tTokenFifo;