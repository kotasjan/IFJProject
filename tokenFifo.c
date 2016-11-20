#include "tokenFifo.h"
#include "scaner.h"
#include "stdio.h"

static tTokenFifo *first = NULL;

int insertTokenFifo(tToken *token)
{
   tTokenFifo *new = calloc(1, sizeof(tTokenFifo));
   if (!new) { return INTERNAL_ERROR; }

   memcpy(&new->token, token, sizeof(tToken));

   if (!first) { first = new; return SUCCESS; }

   tTokenFifo *tmp = first;
   while (tmp->next)
   {
      tmp = tmp->next;
   }

   tmp->next = new;

   return SUCCESS;
}


tToken *getTokenFifo()
{
   if (!first) { return NULL; }
   else 
   {
      tTokenFifo *tmp = first;
      first = first->next;
      return &tmp->token;
   }
}
