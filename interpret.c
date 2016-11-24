#include "interpret.h"
#include "ifj.h"
#include "ial.h"
#include "instruction.h"
#include "parse_new.h"

int interpret(tInstructionList *L){
    //int error = 0;
   // tStackList *stackList = (tStackList*)calloc(1, sizeof(tStackList));
    //stackInstructionInit(stackList);
   // tStackTrees stackTrees;
  //  stackInit(&stackTrees);
   // string tempStr;
   // strInit(&tempStr);
   table *GlobalTable;


    listFirst(L);
    while(L->active != NULL){
        switch (L->active->Instruction->InstrType){

            case I_PUSH_GLOBAL:
            printf("asdsada\n");
               GlobalTable = globalTS;
               L->active = L->active->next;
               break;
         }
      }
   }
