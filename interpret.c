#include "interpret.h"
#include "ifj.h"
#include "instruction.h"

tInstructionItem *instInsert(tInstructionList *instList)
{

   tInstructionItem *instr;
   if ((NULL == (instr = calloc(1, sizeof(tInstructionItem)))))
   {
      return NULL;
   }

   if(!instList->first)
   {
      instList->first = instList->last = instr;
   }
   else
   {
      instr->prev = instList->last;
      instList->last->next = instr;
      instList->last = instr;
   }

   return instr;
}

tInstruction *createInst(tInstrType typ, void *addr1, void *addr2, void *addr3)
{
   tInstruction *newInst;
   if ((NULL == (newInst = malloc(sizeof(tInstruction))))) { return NULL; }

   newInst->instrType = typ;
   newInst->addr1 = addr1;
   newInst->addr2 = addr2;
   newInst->addr3 = addr3;

   return newInst;
}