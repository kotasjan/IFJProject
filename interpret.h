#pragma once
#include "instruction.h"

typedef struct
{
   tInstrType instrType;
   void *addr1;
   void *addr2;
   void *addr3;
} tInstruction;

typedef struct instItem
{
   tInstruction *instruction;
   struct instItem *prev;
   struct instItem *next;
} tInstructionItem;

typedef struct
{
   tInstructionItem *first;
   tInstructionItem *active;
   tInstructionItem *last;   
} tInstructionList;

