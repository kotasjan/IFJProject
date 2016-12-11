#include "interpret.h"
#include "ifj.h"
#include "ial.h"
#include "instruction.h"
#include "parse_new.h"
#include "expr.h"
#include <stdio.h>
#include "debug.h"


int sk = 0;

table *createCopy(table *TS)
{
   int i = 0;

   table * new = calloc(1, sizeof(table));
   if (!new) return NULL;

   for(i = 0; i < 100; i++)
   {
      if((*TS)[i])
      {
         tList *data = (*TS)[i];
         bool ft = true;
         while(data)
         {
            if (data->func)
            {
               //printf("Funkce\n");
            }
            else
            {
              // printf("Promenna\n");
               tVar *var = calloc(1, sizeof(tVar));
               if (!var) return NULL;
               memcpy(var, data->dataPtr, sizeof(tVar));
               var->init = false;
               int res;
               res = tsInsertVar(new, var);
               if (res) exit(res);
            }
            data = data->next;
         }
      }
   }
}

int checkInit(tVar *var)
{
   if (!var->init) debug("INIT\n");
   return var->init ? 0 : 1;
}

struct retList
{
   tInstructionList *List;
   struct retList *next;
};

bool strConCat = false;

char *class = NULL;

struct returnFunc
{
   tVar vys;
   tVar *vysto;
   tInstructionList *List;
   struct returnFunc *next;
};

char concatStr[100000];
char *actPtr = NULL;

struct returnFunc *topReturnFunc = NULL;

//tInstructionList *retList[10];
int i = 0;
static unsigned lnt = 0;

void mathExpr(int exprType, tInstr *Instruction)
{
   switch (exprType)
   {
      case 0:
      {
         switch (Instruction->InstrType)
         {

            case I_ADD:
            {
               debug("I_ADD\n");
               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.intValue + ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue + ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue + ((tValueStack*)Instruction->addr2)->value.intValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.intValue + ((tValueStack*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tValueStack*)Instruction->addr1)->value.intValue + ((tValueStack*)Instruction->addr2)->value.intValue;
               }
            break;
            }
            case I_MINUS:
            {
               debug("I_MINUS\n");
               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tValueStack*)Instruction->addr1)->value.intValue - ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue - (double)((tValueStack*)Instruction->addr2)->value.intValue;
                  }
                   else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue - ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tValueStack*)Instruction->addr1)->value.intValue - (double)((tValueStack*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tValueStack*)Instruction->addr1)->value.intValue - ((tValueStack*)Instruction->addr2)->value.intValue;
               }
            break;
            }
            case I_MUL:
            {
               debug("I_MUL\n");
               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tValueStack*)Instruction->addr1)->value.intValue * ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue * (double)((tValueStack*)Instruction->addr2)->value.intValue;
                  }
                   else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue * ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tValueStack*)Instruction->addr1)->value.intValue * (double)((tValueStack*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tValueStack*)Instruction->addr1)->value.intValue * ((tValueStack*)Instruction->addr2)->value.intValue;
               }
            break;
            }
            case I_DIV:
            {
               debug("I_DIV\n");
               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                     if (!(((tValueStack*)Instruction->addr2)->value.doubleValue)) { debug("Deleni 0\n"); exit(9); }
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tValueStack*)Instruction->addr1)->value.intValue / ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     if (!(((tValueStack*)Instruction->addr2)->value.intValue)) { debug("Deleni 0\n"); exit(9); }
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue / (double)((tValueStack*)Instruction->addr2)->value.intValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      if (!(((tValueStack*)Instruction->addr2)->value.doubleValue)) { debug("Deleni 0\n"); exit(9); }
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue / ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     if (!(((tValueStack*)Instruction->addr2)->value.intValue)) { debug("Deleni 0\n"); exit(9); }
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.intValue / ((tValueStack*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  if (!(((tValueStack*)Instruction->addr2)->value.intValue)) { debug("Deleni 0\n"); exit(9); }
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tValueStack*)Instruction->addr1)->value.intValue / ((tValueStack*)Instruction->addr2)->value.intValue;
               }
            break;
            }
            case I_LESS:
            {
               debug("I_LESS\n");

               if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if ((double)((tValueStack*)Instruction->addr1)->value.intValue < ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue < (double)((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue < ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.intValue < ((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_GREATER:
            {
               debug("I_GREATER\n");
               if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if ((double)((tValueStack*)Instruction->addr1)->value.intValue > ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue > (double)((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue > ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.intValue > ((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }

            break;
            }
            case I_LESS_EQUAL:
            {
               debug("I_LESS_EQUAL\n");
               if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if ((double)((tValueStack*)Instruction->addr1)->value.intValue <= ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue <= (double)((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue <= ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.intValue <= ((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_GREATER_EQUAL:
            {
               debug("I_GREATER_EQUAL\n");
               if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if ((double)((tValueStack*)Instruction->addr1)->value.intValue >= ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue >= (double)((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue >= ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.intValue >= ((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_NOT_EQUAL:
            {
               debug("I_NOT_EQUAL\n");
               if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if ((double)((tValueStack*)Instruction->addr1)->value.intValue != ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue != (double)((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue != ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.intValue != ((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_EQUAL:
            {
               debug("I_EQUAL\n");
               if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if ((double)((tValueStack*)Instruction->addr1)->value.intValue == ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue == (double)((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue == ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.intValue == ((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
         }

      break;

      }
      case 1:
      {
         switch (Instruction->InstrType)
         {
            case I_ADD_FIRST:
            {
               debug("I_ADD_FIRST\n");
               if (checkInit(Instruction->addr1)) exit(8);

               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue + ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue + ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue + (double)((tValueStack*)Instruction->addr2)->value.intValue;
                  }

                  else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue + (double)((tValueStack*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tVar*)Instruction->addr1)->value.intValue + ((tValueStack*)Instruction->addr2)->value.intValue;
               }
            break;
            }
            case I_MINUS_FIRST:
            {
               debug("I_MINUS_FIRST\n");
               if (checkInit(Instruction->addr1)) exit(8);

               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue - ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue - ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue - (double)((tValueStack*)Instruction->addr2)->value.intValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue - (double)((tValueStack*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tVar*)Instruction->addr1)->value.intValue - ((tValueStack*)Instruction->addr2)->value.intValue;
               }
               break;
            }
            case I_MUL_FIRST:
            {
               debug("I_MUL_FIRST\n");
               if (checkInit(Instruction->addr1)) exit(8);

               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue * ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue * ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue * (double)((tValueStack*)Instruction->addr2)->value.intValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue * (double)((tValueStack*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tVar*)Instruction->addr1)->value.intValue * ((tValueStack*)Instruction->addr2)->value.intValue;
               }
               break;
            }
            case I_DIV_FIRST:
            {
               debug("I_DIV_FIRST\n");
               if (checkInit(Instruction->addr1)) exit(8);

               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                     if(!(((tValueStack*)Instruction->addr2)->value.doubleValue)) { debug("Deleni 0\n"); exit(9); }
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue / ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
                  {
                     if(!(((tValueStack*)Instruction->addr2)->value.doubleValue)) { debug("Deleni 0\n"); exit(9); }
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue / ((tValueStack*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     if(!(((tValueStack*)Instruction->addr2)->value.intValue)) { debug("Deleni 0\n"); exit(9); }
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue / (double)((tValueStack*)Instruction->addr2)->value.intValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
                  {
                     if(!(((tValueStack*)Instruction->addr2)->value.intValue)) { debug("Deleni 0\n"); exit(9); }
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.intValue / ((tValueStack*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  if(!(((tValueStack*)Instruction->addr2)->value.intValue)) { debug("Deleni 0\n"); exit(9); }
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tVar*)Instruction->addr1)->value.intValue / ((tValueStack*)Instruction->addr2)->value.intValue;
               }
               break;
            }
            case I_LESS_FIRST:
            {
               debug("I_LESS_FIRST\n");
               if (checkInit(Instruction->addr1)) exit(8);

               if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if ((double)((tVar*)Instruction->addr1)->value.intValue < ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue < (double)((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue < ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.intValue < ((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_GREATER_FIRST:
            {
               debug("I_GREATER_FIRST\n");
               if (checkInit(Instruction->addr1)) exit(8);

               if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if ((double)((tVar*)Instruction->addr1)->value.intValue > ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue > (double)((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue > ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.intValue > ((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_LESS_EQUAL_FIRST:
            {
               debug("I_LESS_EQUAL_FIRST\n");
               if (checkInit(Instruction->addr1)) exit(8);

               if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if ((double)((tVar*)Instruction->addr1)->value.intValue <= ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue <= (double)((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue <= ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.intValue <= ((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_GREATER_EQUAL_FIRST:
            {
               debug("I_GREATER_EQUAL_FIRST\n");
               if (checkInit(Instruction->addr1)) exit(8);

               if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if ((double)((tVar*)Instruction->addr1)->value.intValue >= ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue >= (double)((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue >= ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.intValue >= ((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_NOT_EQUAL_FIRST:
            {
               debug("I_NOT_EQUAL_FIRST\n");
               if (checkInit(Instruction->addr1)) exit(8);

               if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if ((double)((tVar*)Instruction->addr1)->value.intValue != ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue != (double)((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue != ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.intValue != ((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_EQUAL_FIRST:
            {
               debug("I_EQUAL_FIRST\n");
               if (checkInit(Instruction->addr1)) exit(8);

               if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if ((double)((tVar*)Instruction->addr1)->value.intValue == ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue == (double)((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tValueStack*)Instruction->addr2)->typ == TYPE_DOUBLE))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue == ((tValueStack*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tValueStack*)Instruction->addr2)->typ == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.intValue == ((tValueStack*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
         }

      break;

      }
      case 2:
      {
         switch (Instruction->InstrType)
         {
            case I_ADD_SECOND:
            {
               debug("I_ADD_SECOND\n");
               if (checkInit(Instruction->addr2)) exit(8);

               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tValueStack*)Instruction->addr1)->value.intValue + ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue + ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue + (double)((tVar*)Instruction->addr2)->value.intValue;
                  }

                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tValueStack*)Instruction->addr1)->value.intValue + (double)((tVar*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tValueStack*)Instruction->addr1)->value.intValue + ((tVar*)Instruction->addr2)->value.intValue;
               }
               break;
            }
            case I_MINUS_SECOND:
            {
               debug("I_MINUS_SECOND\n");
               if (checkInit(Instruction->addr2)) exit(8);

               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tValueStack*)Instruction->addr1)->value.intValue - ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue - ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue - (double)((tVar*)Instruction->addr2)->value.intValue;
                  }

                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tValueStack*)Instruction->addr1)->value.intValue - (double)((tVar*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tValueStack*)Instruction->addr1)->value.intValue - ((tVar*)Instruction->addr2)->value.intValue;
               }
               break;
            }
            case I_MUL_SECOND:
            {
               debug("I_MUL_SECOND\n");
               if (checkInit(Instruction->addr2)) exit(8);

               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tValueStack*)Instruction->addr1)->value.intValue * ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue * ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue * (double)((tVar*)Instruction->addr2)->value.intValue;
                  }

                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tValueStack*)Instruction->addr1)->value.intValue * (double)((tVar*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tValueStack*)Instruction->addr1)->value.intValue * ((tVar*)Instruction->addr2)->value.intValue;
               }
               break;
            }
            case I_DIV_SECOND:
            {
               debug("I_DIV_SECOND\n");
               if (checkInit(Instruction->addr2)) exit(8);

               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                     if (!(((tVar*)Instruction->addr2)->value.doubleValue)) { debug("Deleni 0\n"); exit(9); }
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tValueStack*)Instruction->addr1)->value.intValue / ((tVar*)Instruction->addr2)->value.doubleValue;
                     
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                     if (!(((tVar*)Instruction->addr2)->value.doubleValue)) { debug("Deleni 0\n"); exit(9); }
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue / ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     if (!(((tVar*)Instruction->addr2)->value.intValue)) { debug("Deleni 0\n"); exit(9); }
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.doubleValue / (double)((tVar*)Instruction->addr2)->value.intValue;
                  }

                  else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     if (!(((tVar*)Instruction->addr2)->value.intValue)) { debug("Deleni 0\n"); exit(9); }
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tValueStack*)Instruction->addr1)->value.intValue / ((tVar*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  if (!(((tVar*)Instruction->addr2)->value.intValue)) { debug("Deleni 0\n"); exit(9); }
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tValueStack*)Instruction->addr1)->value.intValue / ((tVar*)Instruction->addr2)->value.intValue;
               }
               break;
            }
            case I_LESS_SECOND:
            {
               debug("I_LESS_SECOND\n");
               if (checkInit(Instruction->addr2)) exit(8);

               if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if ((double)((tValueStack*)Instruction->addr1)->value.intValue < ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue < (double)((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue < ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.intValue < ((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_GREATER_SECOND:
            {
               debug("I_GREATER_SECOND\n");
               if (checkInit(Instruction->addr2)) exit(8);

               if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if ((double)((tValueStack*)Instruction->addr1)->value.intValue > ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue > (double)((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue > ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.intValue > ((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_LESS_EQUAL_SECOND:
            {
               debug("I_LESS_EQUAL_SECOND\n");
               if (checkInit(Instruction->addr2)) exit(8);

               if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if ((double)((tValueStack*)Instruction->addr1)->value.intValue <= ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue <= (double)((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue <= ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.intValue <= ((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_GREATER_EQUAL_SECOND:
            {
               debug("I_GREATER_EQUAL_SECOND\n");
               if (checkInit(Instruction->addr2)) exit(8);

               if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if ((double)((tValueStack*)Instruction->addr1)->value.intValue >= ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue >= (double)((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue >= ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.intValue >= ((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_NOT_EQUAL_SECOND:
            {
               debug("I_NOT_EQUAL_SECOND\n");
               if (checkInit(Instruction->addr2)) exit(8);

               if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if ((double)((tValueStack*)Instruction->addr1)->value.intValue != ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue != (double)((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue != ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.intValue != ((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_EQUAL_SECOND:
            {
               debug("I_EQUAL_SECOND\n");
               if (checkInit(Instruction->addr2)) exit(8);

               if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if ((double)((tValueStack*)Instruction->addr1)->value.intValue == ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue == (double)((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if (((tValueStack*)Instruction->addr1)->value.doubleValue == ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tValueStack*)Instruction->addr1)->typ == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tValueStack*)Instruction->addr1)->value.intValue == ((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
         }

      break;

      }
      case 3:
      {
         switch (Instruction->InstrType)
         {
            case I_ADD_BOTH:
            {
               debug("I_ADD_BOTH\n");
               if (checkInit(Instruction->addr2)) exit(8);
               if (checkInit(Instruction->addr1)) exit(8);

               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue + ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue + ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue + (double)((tVar*)Instruction->addr2)->value.intValue;
                  }

                  else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue + (double)((tVar*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tVar*)Instruction->addr1)->value.intValue + ((tVar*)Instruction->addr2)->value.intValue;
               }
               break;
            }
            case I_MINUS_BOTH:
            {
               debug("I_MINUS_BOTH\n");
               if (checkInit(Instruction->addr2)) exit(8);
               if (checkInit(Instruction->addr1)) exit(8);

               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue - ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue - ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue - (double)((tVar*)Instruction->addr2)->value.intValue;
                  }

                  else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue - (double)((tVar*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tVar*)Instruction->addr1)->value.intValue - ((tVar*)Instruction->addr2)->value.intValue;
               }
               break;
            }
            case I_MUL_BOTH:
            {
               debug("I_MUL_BOTH\n");
               if (checkInit(Instruction->addr2)) exit(8);
               if (checkInit(Instruction->addr1)) exit(8);

               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue * ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue * ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue * (double)((tVar*)Instruction->addr2)->value.intValue;
                  }

                  else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue * (double)((tVar*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tVar*)Instruction->addr1)->value.intValue * ((tVar*)Instruction->addr2)->value.intValue;
               }
               break;
            }
            case I_DIV_BOTH:
            {
               debug("I_DIV_BOTH\n");
               if (checkInit(Instruction->addr2)) exit(8);
               if (checkInit(Instruction->addr1)) exit(8);

               if (((tValueStack*)Instruction->addr3)->typ == TYPE_DOUBLE)
               {
                  if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                     if(!(((tVar*)Instruction->addr2)->value.doubleValue)) { debug("Deleni 0\n"); exit(9); }
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue / ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
                  {
                     if(!(((tVar*)Instruction->addr2)->value.doubleValue)) { debug("Deleni 0\n"); exit(9); }
                      ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue / ((tVar*)Instruction->addr2)->value.doubleValue;
                  }
                  else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     if(!(((tVar*)Instruction->addr2)->value.intValue)) { debug("Deleni 0\n"); exit(9); }
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = ((tVar*)Instruction->addr1)->value.doubleValue / (double)((tVar*)Instruction->addr2)->value.intValue;
                  }

                  else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
                  {
                     if(!(((tVar*)Instruction->addr2)->value.intValue)) { debug("Deleni 0\n"); exit(9); }
                     ((tValueStack*)Instruction->addr3)->value.doubleValue = (double)((tVar*)Instruction->addr1)->value.intValue / (double)((tVar*)Instruction->addr2)->value.intValue;
                  }
               }
               else if (((tValueStack*)Instruction->addr3)->typ == TYPE_INT)
               {
                  if(!(((tVar*)Instruction->addr2)->value.intValue)) { debug("Deleni 0\n"); exit(9); }
                  ((tValueStack*)Instruction->addr3)->value.intValue = ((tVar*)Instruction->addr1)->value.intValue / ((tVar*)Instruction->addr2)->value.intValue;
               }
               break;
            }
            case I_LESS_BOTH:
            {
               debug("I_LESS_BOTH\n");
               if (checkInit(Instruction->addr2)) exit(8);
               if (checkInit(Instruction->addr1)) exit(8);

               if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if ((double)((tVar*)Instruction->addr1)->value.intValue < ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue < (double)((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue < ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.intValue < ((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_GREATER_BOTH:
            {
               debug("I_GREATER_BOTH\n");
               if (checkInit(Instruction->addr2)) exit(8);
               if (checkInit(Instruction->addr1)) exit(8);

               if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if ((double)((tVar*)Instruction->addr1)->value.intValue > ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue > (double)((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue > ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.intValue > ((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_LESS_EQUAL_BOTH:
            {
               debug("I_LESS_EQUAL_BOTH\n");
               if (checkInit(Instruction->addr2)) exit(8);
               if (checkInit(Instruction->addr1)) exit(8);

               if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if ((double)((tVar*)Instruction->addr1)->value.intValue <= ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue <= (double)((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue <= ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.intValue <= ((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_GREATER_EQUAL_BOTH:
            {
               debug("I_GREATER_EQUAL_BOTH\n");
               if (checkInit(Instruction->addr2)) exit(8);
               if (checkInit(Instruction->addr1)) exit(8);

               if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if ((double)((tVar*)Instruction->addr1)->value.intValue >= ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue >= (double)((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue >= ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.intValue >= ((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_NOT_EQUAL_BOTH:
            {
               debug("I_NOT_EQUAL_BOTH\n");
               if (checkInit(Instruction->addr2)) exit(8);
               if (checkInit(Instruction->addr1)) exit(8);

               if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if ((double)((tVar*)Instruction->addr1)->value.intValue != ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue != (double)((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue != ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.intValue != ((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
            case I_EQUAL_BOTH:
            {
               debug("I_EQUAL_BOTH\n");
               if (checkInit(Instruction->addr2)) exit(8);
               if (checkInit(Instruction->addr1)) exit(8);

               if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if ((double)((tVar*)Instruction->addr1)->value.intValue == ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue == (double)((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_DOUBLE) && (((tVar*)Instruction->addr2)->type == TYPE_DOUBLE))
               {
                  if (((tVar*)Instruction->addr1)->value.doubleValue == ((tVar*)Instruction->addr2)->value.doubleValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
               else if((((tVar*)Instruction->addr1)->type == TYPE_INT) && (((tVar*)Instruction->addr2)->type == TYPE_INT))
               {
                  if (((tVar*)Instruction->addr1)->value.intValue == ((tVar*)Instruction->addr2)->value.intValue)
                     ((tValueStack*)Instruction->addr3)->value.intValue = 1;
                  else
                     ((tValueStack*)Instruction->addr3)->value.intValue = 0;
               }
            break;
            }
         }

      break;

      }
   }
}

int interpret(tInstructionList *L){
    //int error = 0;
   // tStackList *stackList = (tStackList*)calloc(1, sizeof(tStackList));
    //stackInstructionInit(stackList);
   // tStackTrees stackTrees;
  //  stackInit(&stackTrees);
   // string tempStr;
   // strInit(&tempStr);

  // tInstructionList *retList;
   struct retList *top = NULL;
   bool returnCode = false;


    listFirst(L);

    while(L->active != NULL){
        switch (L->active->Instruction->InstrType){
          case I_PUSH:
            //printf("I_PUSH\n");
            //printf("TOTOT %d \n",  ((tVar*)L->active->Instruction->addr3)->value.intValue);
            L->active = L->active->next;

          break;

          case I_CONCAT:

           // printf("CONCAT=\n");
            if (!((tVar*)L->active->Instruction->addr1)->init)
            {
               debug("INIT\n");
               exit(8);
            }
            //printf("%d LNT\n",lnt );
            switch(((tVar*)L->active->Instruction->addr1)->type)
            {
               case TYPE_INT:
               {
                  debug("TYPE_INT\n");
                  sprintf(&concatStr[lnt], "%d",((tVar*)L->active->Instruction->addr1)->value.intValue );
                 // printf("%s %d \n",concatStr , ((tVar*)L->active->Instruction->addr1)->value.intValue);
                  for(i = 0; i < 100000; i++)
                  {
                     if (concatStr[i] == '\0')
                     {
                       // printf("INDEX %d\n",i );
                        lnt = i;
                        break;
                     }
                  }

                  break;
               }
               case TYPE_DOUBLE:
               {
                  debug("TYPE_DOUBLE\n");
                  sprintf(&concatStr[lnt], "%f",((tVar*)L->active->Instruction->addr1)->value.doubleValue );
                  //printf("%s %f \n",concatStr , ((tVar*)L->active->Instruction->addr1)->value.doubleValue);
                  for(i = 0; i < 100000; i++)
                  {
                     if (concatStr[i] == '\0')
                     {
                       // printf("INDEX %d\n",i );
                        lnt = i;
                        break;
                     }
                  }

                  break;
               }
               case TYPE_STRING:
               {
                  debug("TYPE_STRING\n");
                  sprintf(&concatStr[lnt], "%s",((tVar*)L->active->Instruction->addr1)->value.stringValue );
                 // printf("%s %s \n",concatStr , ((tVar*)L->active->Instruction->addr1)->value.stringValue);
                  for(i = 0; i < 100000; i++)
                  {
                     if (concatStr[i] == '\0')
                     {
                        //printf("INDEX %d\n",i );
                        lnt = i;
                        break;
                     }
                  }

                  break;
               }

            }
            L->active = L->active ->next;
            break;

          case I_ASSIGNMENT:
            //printf("I_ASSIGNMENT\n");
          
          lnt = 0;

           if (strConCat)
           {
               ((tToken*)L->active->Instruction->addr1)->type = TYPE_STRING;
               ((tVar*)L->active->Instruction->addr3)->init = true;
               ((tVar*)L->active->Instruction->addr3)->value.stringValue = calloc(strlen(concatStr)+1, sizeof(char));
               if (!((tVar*)L->active->Instruction->addr3)->value.stringValue) return INTERNAL_ERROR;
               int strlnt = strlen(concatStr);
               memcpy(((tVar*)L->active->Instruction->addr3)->value.stringValue, concatStr, strlnt+1);
               strConCat = false;
               lnt = 0;
               memset(concatStr, 0, 100000);
              // printf("HODNOTA STRING %s \n", ((tVar*)L->active->Instruction->addr3)->value.stringValue );
               L->active = L->active->next;
              //  printf("56565\n");
               break;

               
           }

            if (((tToken*)L->active->Instruction->addr1)->type != ((tVar*)L->active->Instruction->addr3)->type)
            {
            // TODO
               if ( (((tToken*)L->active->Instruction->addr1)->type == TYPE_INT) && (((tVar*)L->active->Instruction->addr3)->type == TYPE_DOUBLE)) 
               {
                  debug("Pretypovavam int na double\n");
                  ((tToken*)L->active->Instruction->addr1)->type = TYPE_DOUBLE;
                  ((tToken*)L->active->Instruction->addr1)->value.doubleValue = (double)((tToken*)L->active->Instruction->addr1)->value.intValue;
               }
               else
               {
                  debug("Nerovnaji se datove typy %d %d \n", ((tToken*)L->active->Instruction->addr1)->type, ((tVar*)L->active->Instruction->addr3)->type); // asi nejaka chyba
                  exit(4);
               }
            }

            if (((tToken*)L->active->Instruction->addr1)->type == TYPE_INT)
            {
              ((tVar*)L->active->Instruction->addr3)->value.intValue = ((tToken*)L->active->Instruction->addr1)->value.intValue;
             // printf("HODNOTA INT %d\n", ((tVar*)L->active->Instruction->addr3)->value.intValue);
            }
            else if (((tToken*)L->active->Instruction->addr1)->type == TYPE_DOUBLE)
            {
              ((tVar*)L->active->Instruction->addr3)->value.doubleValue = ((tToken*)L->active->Instruction->addr1)->value.doubleValue;
             // printf("HODNOTA DOUBLE %f\n", ((tVar*)L->active->Instruction->addr3)->value.doubleValue);
            }
            else if (((tToken*)L->active->Instruction->addr1)->type == TYPE_STRING)
            {
              ((tVar*)L->active->Instruction->addr3)->value.stringValue = ((tToken*)L->active->Instruction->addr1)->value.stringValue;
             // printf("HODNOTA STRING %s\n", ((tVar*)L->active->Instruction->addr3)->value.stringValue);
            }

            ((tVar*)L->active->Instruction->addr3)->init = true;


            strConCat = false;
            lnt = 0;
            memset(concatStr, 0, 100000);
            L->active = L->active->next;

          break;

          case I_ASSIGN_EXPR:
            //printf("I_ASSIGN_EXPR\n");

            if(((tValueStack*)L->active->Instruction->addr1)->typ == INT )
            {
               if (L->active->Instruction->addr2)
                  ((tToken*)L->active->Instruction->addr3)->value.intValue = ((tValueStack*)L->active->Instruction->addr1)->var->value.intValue;
               else
                  ((tToken*)L->active->Instruction->addr3)->value.intValue = ((tValueStack*)L->active->Instruction->addr1)->value.intValue;
            }
            else if(((tValueStack*)L->active->Instruction->addr1)->typ == DOUBLE )
            {
               if (L->active->Instruction->addr2)
                  ((tToken*)L->active->Instruction->addr3)->value.doubleValue = ((tValueStack*)L->active->Instruction->addr1)->var->value.doubleValue;
               else
                  ((tToken*)L->active->Instruction->addr3)->value.doubleValue = ((tValueStack*)L->active->Instruction->addr1)->value.doubleValue;
            }
            else if(((tValueStack*)L->active->Instruction->addr1)->typ == STRING )
            {
               if (L->active->Instruction->addr2)
                  ((tToken*)L->active->Instruction->addr3)->value.stringValue = ((tValueStack*)L->active->Instruction->addr1)->var->value.stringValue;
               else
                  ((tToken*)L->active->Instruction->addr3)->value.stringValue = ((tValueStack*)L->active->Instruction->addr1)->value.stringValue;

            }

            ((tToken*)L->active->Instruction->addr3)->type = ((tValueStack*)L->active->Instruction->addr1)->typ;
            L->active = L->active->next;

          break;

          case I_MUL:
          case I_DIV:
          case I_MINUS:
            strConCat = false;
          case I_ADD:
            mathExpr(0, L->active->Instruction);
            L->active = L->active->next;
          break;

          case I_MINUS_FIRST:
          case I_ADD_FIRST:
          case I_MUL_FIRST:
          case I_DIV_FIRST:
            mathExpr(1, L->active->Instruction);
            L->active = L->active->next;
         break;

          case I_MINUS_SECOND:
          case I_ADD_SECOND:
          case I_MUL_SECOND:
          case I_DIV_SECOND:
            mathExpr(2, L->active->Instruction);
            L->active = L->active->next;
         break;

          case I_MINUS_BOTH:
          case I_ADD_BOTH:
          case I_MUL_BOTH:
          case I_DIV_BOTH:
            mathExpr(3, L->active->Instruction);
            L->active = L->active->next;
         break;
           
          case I_LESS:
          case I_GREATER:
          case I_LESS_EQUAL:
          case I_GREATER_EQUAL:
          case I_NOT_EQUAL:
          case I_EQUAL:
            mathExpr(0, L->active->Instruction);
            L->active = L->active->next;
         break;

          case I_LESS_FIRST:
          case I_GREATER_FIRST:
          case I_LESS_EQUAL_FIRST:
          case I_GREATER_EQUAL_FIRST:
          case I_NOT_EQUAL_FIRST:
          case I_EQUAL_FIRST:
            mathExpr(1, L->active->Instruction);
            L->active = L->active->next;
         break;

          case I_LESS_SECOND:
          case I_GREATER_SECOND:
          case I_LESS_EQUAL_SECOND:
          case I_GREATER_EQUAL_SECOND:
          case I_NOT_EQUAL_SECOND:
          case I_EQUAL_SECOND:
            mathExpr(2, L->active->Instruction);
            L->active = L->active->next;
         break;

          case I_LESS_BOTH:
          case I_GREATER_BOTH:
          case I_LESS_EQUAL_BOTH:
          case I_GREATER_EQUAL_BOTH:
          case I_NOT_EQUAL_BOTH:
          case I_EQUAL_BOTH:
            mathExpr(3, L->active->Instruction);
            L->active = L->active->next;
         break;

          case I_SET_START:
            //printf("I_SET_START\n");
              L = ((tFunc*)L->active->Instruction->addr3)->List;
              listFirst(L);
          break;

         case I_IF:
         sk++;

         debug("I_IF\n");
            if (!top)
            {
               struct retList * retList = calloc(1, sizeof(struct retList));
               if (!retList) return INTERNAL_ERROR;
               top = retList;
               retList->List = L->active->next;
            }
            else
            {
               struct retList * retList = calloc(1, sizeof(struct retList));
               if (!retList) return INTERNAL_ERROR;
               retList->next = top;
               top = retList;
               retList->List = L->active->next;

            }

            if (((tToken*)L->active->Instruction->addr1)->value.intValue)
            {
               //printf("VETEV IF\n");
               L->active = listFirst(L->active->Instruction->addr2);
               
            }
            else
            {
               //printf("VETEV ELSE\n");
               L->active = listFirst(L->active->Instruction->addr3);  
            }
            //L->active = L->active->next;
         break;

         case I_TEST:
            L->active = L->active->next;
            break;

         case I_SKIP_TAPE:
         sk--;
            debug("I_SKIP_TAPE\n");
            L->active = top->List;
            //printf("skip\n");
            top = top->next;
            break;

         
         case I_WHILE:

            sk++;
            if (!top)
            {
               struct retList * retList = calloc(1, sizeof(struct retList));
               if (!retList) return INTERNAL_ERROR;
               top = retList;
               retList->List = L->active->next;
            }
            else
            {
               struct retList * retList = calloc(1, sizeof(struct retList));
               if (!retList) return INTERNAL_ERROR;
               retList->next = top;
               top = retList;
               retList->List = L->active->next;

            }
            if (((tToken*)L->active->Instruction->addr1)->value.intValue)
            {
               //printf("VETEV WHILE TRUE\n");
               L->active = listFirst(L->active->Instruction->addr2);
               
            }
            else
            {
               top = top->next;
               //printf("VETEV WHILE FALSE\n");
               L->active = L->active->Instruction->addr3;  
            }

         break;

         case I_SKIP_NEXT_INST:
            L->active = L->active->next;
         break;

         case I_SKIP_BEFORE_WHILE:
             L->active = L->active->Instruction->addr1;
         break;

         

         case I_SET_CLASS:
            class = L->active->Instruction->addr1;
            L->active = L->active->next;

         break;


         case I_CALL:
         {
            debug("I_CALL\n");
            int paramcnt = 0;
            debug("Volani funkce\n");
            debug("Volani funkce %s\n", ((tFunc*)L->active->Instruction->addr1)->name );
            debug("Volani funkce %s\n", ((tFunc*)L->active->Instruction->addr1)->name );

            bool skip_ins = true;
            if(class && (!strcmp(class, "ifj16")))
            {
                  returnCode = true;
                  if(!strcmp(((tFunc*)L->active->Instruction->addr1)->name , "length"))
                  {
                     if (((tVar**)L->active->Instruction->addr2)[0]->type != TYPE_STRING) exit(4); //TODO
                     if(checkInit(((tVar**)L->active->Instruction->addr2)[0])) { debug("INIT\n"); exit(8); }
                     ((tVar*)L->active->Instruction->addr3)->value.intValue = length(((tVar**)L->active->Instruction->addr2)[i]->value.stringValue);
                     debug("Funkce Length %d \n", ((tVar*)L->active->Instruction->addr3)->value.intValue);
                  }
                  else  if(!strcmp(((tFunc*)L->active->Instruction->addr1)->name , "readInt"))
                  {
                     ((tVar*)L->active->Instruction->addr3)->value.intValue = readInt();
                     debug("Funkce readInt %d \n", ((tVar*)L->active->Instruction->addr3)->value.intValue);

                  }
                  else if(!strcmp(((tFunc*)L->active->Instruction->addr1)->name , "readString"))
                  {
                     ((tVar*)L->active->Instruction->addr3)->value.stringValue = readString();
                     debug("Funkce readString %s  ggg", ((tVar*)L->active->Instruction->addr3)->value.stringValue);

                  }
                  else  if(!strcmp(((tFunc*)L->active->Instruction->addr1)->name , "readDouble"))
                  {
                     ((tVar*)L->active->Instruction->addr3)->value.doubleValue = readDouble();

                     debug("Funkce reaaDouble %f \n", ((tVar*)L->active->Instruction->addr3)->value.doubleValue);

                  }
                  else  if(!strcmp(((tFunc*)L->active->Instruction->addr1)->name , "substr"))
                  {
                     if (((tVar**)L->active->Instruction->addr2)[0]->type != TYPE_STRING) return 4;
                     if (((tVar**)L->active->Instruction->addr2)[1]->type != TYPE_INT) return 4;
                     if (((tVar**)L->active->Instruction->addr2)[2]->type != TYPE_INT) return 4;
                     if(checkInit(((tVar**)L->active->Instruction->addr2)[0])) { debug("INIT\n"); exit(8); }
                     if( checkInit(((tVar**)L->active->Instruction->addr2)[1])) { debug("INIT\n"); exit(8); }
                     if(checkInit(((tVar**)L->active->Instruction->addr2)[2])) { debug("INIT\n"); exit(8); }


                     ((tVar*)L->active->Instruction->addr3)->value.stringValue = substr(((tVar**)L->active->Instruction->addr2)[0]->value.stringValue,
                                                                                       ((tVar**)L->active->Instruction->addr2)[1]->value.intValue,
                                                                                       ((tVar**)L->active->Instruction->addr2)[2]->value.intValue);
                  }
                  else  if(!strcmp(((tFunc*)L->active->Instruction->addr1)->name , "compare"))
                  {
                     if (((tVar**)L->active->Instruction->addr2)[0]->type != TYPE_STRING) return 4;
                     if (((tVar**)L->active->Instruction->addr2)[1]->type != TYPE_STRING) return 4;

                     if(checkInit(((tVar**)L->active->Instruction->addr2)[0])) { debug("INIT\n"); exit(8); }
                     if( checkInit(((tVar**)L->active->Instruction->addr2)[1])) { debug("INIT\n"); exit(8); }
                     ((tVar*)L->active->Instruction->addr3)->value.intValue = compare(((tVar**)L->active->Instruction->addr2)[0]->value.stringValue,
                                                                                       ((tVar**)L->active->Instruction->addr2)[1]->value.stringValue);

                  }
                  else  if(!strcmp(((tFunc*)L->active->Instruction->addr1)->name , "find"))
                  {
                     if (((tVar**)L->active->Instruction->addr2)[0]->type != TYPE_STRING) return 4;
                     if (((tVar**)L->active->Instruction->addr2)[1]->type != TYPE_STRING) return 4;

                     if(checkInit(((tVar**)L->active->Instruction->addr2)[0])) { debug("INIT\n"); exit(8); }
                     if( checkInit(((tVar**)L->active->Instruction->addr2)[1])) { debug("INIT\n"); exit(8); }

                     ((tVar*)L->active->Instruction->addr3)->value.intValue = find(((tVar**)L->active->Instruction->addr2)[0]->value.stringValue,
                                                                                       ((tVar**)L->active->Instruction->addr2)[1]->value.stringValue);

                  }
                  else  if(!strcmp(((tFunc*)L->active->Instruction->addr1)->name , "sort"))
                  {
                     if (((tVar**)L->active->Instruction->addr2)[0]->type != TYPE_STRING) return 4;
                     if(checkInit(((tVar**)L->active->Instruction->addr2)[0])) { debug("INIT\n"); exit(8); }

                     ((tVar*)L->active->Instruction->addr3)->value.stringValue = sort(((tVar**)L->active->Instruction->addr2)[0]->value.stringValue);

                  }
                  else  if(!strcmp(((tFunc*)L->active->Instruction->addr1)->name , "print"))
                  {

                  }
                  ((tVar*)L->active->Instruction->addr3)->init = true;
            }
            else
            {
               skip_ins = false;

               sk = 0;

               int i = 0;
               while (i != ((tFunc*)L->active->Instruction->addr1)->paramCnt)
               {
                  if (((tVar**)L->active->Instruction->addr2)[i]->type == TYPE_STRING)
                  {
                     debug("Parametr %d: typ STRING %s\n", i, ((tVar**)L->active->Instruction->addr2)[i]->value.stringValue);
                     tFuncParam *param = ((tFunc*)L->active->Instruction->addr1)->param; 
                     for(int o = 0; o < ((tFunc*)L->active->Instruction->addr1)->paramCnt; o++)
                     {
                        if (o == i)
                           break;
                        param = param->nextParam;
                     }
                     tList *data = tsRead(((tFunc*)L->active->Instruction->addr1)->funcTable, param->id);
                     tVar *var = data->dataPtr;
                     var->value.stringValue = ((tVar**)L->active->Instruction->addr2)[i]->value.stringValue;
                  }
                  else if (((tVar**)L->active->Instruction->addr2)[i]->type == TYPE_INT)
                  {
                     debug("Parametr %d: typ INT %d\n",i, ((tVar**)L->active->Instruction->addr2)[i]->value.intValue);
                     tFuncParam *param = ((tFunc*)L->active->Instruction->addr1)->param; 
                     for(int o = 0; o < ((tFunc*)L->active->Instruction->addr1)->paramCnt; o++)
                     {
                        if (o == i)
                           break;
                        param = param->nextParam;
                     }
                     tList *data = tsRead(((tFunc*)L->active->Instruction->addr1)->funcTable, param->id);
                     tVar *var = data->dataPtr;
                     var->value.intValue = ((tVar**)L->active->Instruction->addr2)[i]->value.intValue;
                  }
                  else if (((tVar**)L->active->Instruction->addr2)[i]->type == TYPE_DOUBLE)
                  {
                     debug("Parametr %d: typ DOUBLE %f\n",i, ((tVar**)L->active->Instruction->addr2)[i]->value.doubleValue);
                     tFuncParam *param = ((tFunc*)L->active->Instruction->addr1)->param; 
                     for(int o = 0; o < ((tFunc*)L->active->Instruction->addr1)->paramCnt; o++)
                     {
                        if (o == i)
                           break;
                        param = param->nextParam;
                     }
                     tList *data = tsRead(((tFunc*)L->active->Instruction->addr1)->funcTable, param->id);
                     tVar *var = data->dataPtr;
                     var->value.doubleValue = ((tVar**)L->active->Instruction->addr2)[i]->value.doubleValue;
                  }
                  i++;
               }

               tVar *var;
               if (NULL == (var = calloc(1, sizeof(tVar)))) { return INTERNAL_ERROR; }
               var->type = TYPE_UNDEF;
               var->id = "@return";
               var->init = false;
               tList *data = tsRead(((tFunc*)L->active->Instruction->addr1)->funcTable, "@return");
               if (!data)
                  if ((tsInsertVar(((tFunc*)L->active->Instruction->addr1)->funcTable, var))) { return INTERNAL_ERROR; }
               else
                  var = data->dataPtr;

               struct returnFunc *ret;
               if (!topReturnFunc)
               {
                 // printf("sss\n");
                  ret = calloc(1, sizeof(struct returnFunc));
                  if (!ret) return INTERNAL_ERROR;
                  topReturnFunc = ret;
               }
               else
               {
                 // printf("666\n");
                  ret = calloc(1, sizeof(struct returnFunc));
                  if (!ret) return INTERNAL_ERROR;
                  ret->next = topReturnFunc;
                  topReturnFunc = ret;
               }

               ret->vysto = L->active->Instruction->addr3;
               ret->List = L->active->next;
               debug("NOVA UROVEN\n");
               L->active = listFirst(((tFunc*)L->active->Instruction->addr1)->List);
            }

            

            class = NULL;
            if (skip_ins)
             L->active = L->active->next;
         break;
      }

      case I_SKIP:
      debug("I_SKIP\n");
         //printf("skipp\n");
         if (!topReturnFunc) { debug("Konec programu\n"); exit(0);}
         tVar var1 = topReturnFunc->vys;
         tVar *var2 = topReturnFunc->vysto;
         //printf("%d %d \n",var2->type , var1.type );
         if (var2->type == TYPE_VOID && !var1.type) { debug("Funkce void bez return\n"); }
         else if (var2->type != var1.type) { debug("ret type\n"); exit(8); }
         //topReturnFunc = topReturnFunc->next;
         L->active = topReturnFunc->List;
         // MAYBE
        //topReturnFunc = topReturnFunc->next;
         ////printf("%d %d \n",var1.type, var2->type );
         break;


      case I_RETURN:
      {
         debug("I_RETURN\n");

         tList *data = tsRead(L->active->Instruction->addr2, "@return");

         if (!data && L->active->Instruction->addr1) { debug("Return v main.run()\n"); return 8;}
         if (!data) {debug("Konec programu\n"); exit(0);}

         tVar *var = data->dataPtr;
         if (L->active->Instruction->addr1)
         {
            //printf("skdalsds %d \n",  ((tToken*)L->active->Instruction->addr1)->type);
            var->type = ((tToken*)L->active->Instruction->addr1)->type;
         }
         else
         {
           // printf("VOID\n");
            var->type = TYPE_VOID;
            //returnCode = false;
            //L->active = topReturnFunc->List; //L->active->next;

            //topReturnFunc = topReturnFunc->next;

           // break;
         }

        // if(!(topReturnFunc))
           // printf("NULL\n");
         topReturnFunc->vys.type = var->type;
         var->id = "@return";

         if(var->type == TYPE_INT)
         {
           // printf("rett code %d \n",  ((tToken*)L->active->Instruction->addr1)->value.intValue);
            topReturnFunc->vys.value.intValue = var->value.intValue = ((tToken*)L->active->Instruction->addr1)->value.intValue;
         }
         else if(var->type == TYPE_DOUBLE)
            topReturnFunc->vys.value.doubleValue = var->value.doubleValue = ((tToken*)L->active->Instruction->addr1)->value.doubleValue;
         else if(var->type == TYPE_STRING)
         {
          //  printf("sem u s dasd\n");
            char *tmp;
   

               tmp = calloc(strlen(concatStr)+1, sizeof(char));
               if (!tmp) return INTERNAL_ERROR;
               int strlnt = strlen(concatStr);
               memcpy(tmp, concatStr, strlnt+1);
               strConCat = false;
               lnt = 0;
               memset(concatStr, 0, 100000);
             //  printf("HODNOTA STRING %s \n", tmp );
               
           


            topReturnFunc->vys.value.stringValue = var->value.stringValue = tmp;
           // printf("%s  tmp\n",tmp );
         }
         

         //printf("I_RETURN\n");
         returnCode = true;
         L->active = topReturnFunc->List; //L->active->next;

         //topReturnFunc = topReturnFunc->next;

         break;

      }
    
      case I_FUNC_CMP:
      {
         debug("I_FUNC_CMP\n");
         tVar var1 = topReturnFunc->vys;
         tVar *var2 = topReturnFunc->vysto;
         //printf("SKSKSSKKSKSK------------------ %d\n",sk );
         int c = 0;

         // OPRAVA RETURNU V IFU
         while(c < sk)
         {
            //L->active = top->List;
            //printf("skip\n");
            top = top->next;
            c++;
         }




         //printf("aa %d %d \n",var1.type, var2->type );
        debug("SKIP TOP\n\n");
         topReturnFunc = topReturnFunc->next;
         if (var2->type == TYPE_VOID && !var1.type) { debug("Funkce void bez return\n"); }
         else
         {
            if(var1.type != var2->type) 
            {
               debug("Nerovnaji se navratove typyv %d %d\n", var1.type ,var2->type);
               return 10;
            }

            if(var1.type == TYPE_INT)
            {
               //printf("typ int ret %d \n", var1.value.intValue);
               var2->value.intValue = var1.value.intValue;
               var2->init = true;
            }
            else if(var1.type == TYPE_DOUBLE)
            {
               var2->value.doubleValue = var1.value.doubleValue;
               var2->init = true;
            }
            else if(var1.type == TYPE_STRING)
            {
               var2->value.stringValue = var1.value.stringValue;
               var2->init = true;
            }
         }
         ////printf(" %d %d\n",var1.type, var2->type );
          //printf("I_FUNC_CMP\n");
          L->active = L->active->next;

         break;
      }


      case I_PRINT:
      {
         switch (((tVar*)L->active->Instruction->addr3)->type)
         {
            case TYPE_INT:
               if(checkInit((tVar*)L->active->Instruction->addr3)) { debug("INIT\n"); exit(8); }
               print_int(((tVar*)L->active->Instruction->addr3)->value.intValue);
               break;
            case TYPE_DOUBLE:
               if(checkInit((tVar*)L->active->Instruction->addr3)) { debug("INIT\n"); exit(8); }
               print_double(((tVar*)L->active->Instruction->addr3)->value.doubleValue);
               break;
            case TYPE_STRING:
               if(checkInit((tVar*)L->active->Instruction->addr3)) { debug("INIT\n"); exit(8); }
               print_string(((tVar*)L->active->Instruction->addr3)->value.stringValue);
               break;
         }

        // //printf("I_PRINT\n");
         L->active = L->active->next;
         break;
      }

      case I_SET_RET:
      {

         debug("I_SET_RETURN\n");
         if (L->active->Instruction->addr3 && !returnCode)
         {
            returnCode = false;
            L->active = L->active->next;
         }
         else
         {

            if (!returnCode) {debug("Neni return\n"); exit(8); }
            returnCode = false;
            L->active = L->active->next;
         }
         break;
      }

      case I_SET_CONCAT:
         strConCat = true;
         L->active = L->active->next;
          
      
   }
}
}