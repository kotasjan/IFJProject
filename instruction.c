#include "instruction.h"
#include <stdlib.h>
#include <stdio.h>
#include "expr.h"
#include "scaner.h"

tInstructionList *ActualList = NULL;
tInstr *generateInstruction(tInstrType type, void *addr1, void *addr2, void *addr3){
    tInstr *I = (tInstr*) calloc(1, sizeof(tInstr));
    if (!I) { return NULL;  }
    I->InstrType = type;
    I->addr1 = addr1;
    I->addr2 = addr2;
    I->addr3 = addr3;
    return I;
}


void freeInstruction(tInstr *I){
    free(I);
}


tInstructionList *listInit(){
    tInstructionList *tList = (tInstructionList*) calloc(1, sizeof(tInstructionList));
    if (!tList) { return NULL; }
    tList->first= NULL;
    tList->last= NULL;
    tList->active= NULL;
    ActualList = tList;
    return tList;
}


void freeList(tInstructionList *L){
    tInstructionItem *ptr;
    while(L->first){
        ptr = L->first;
        L->first = L->first->next;
        freeInstruction(ptr->Instruction);
        free(ptr);
    }
    free(L);
}

void listFirst(tInstructionList *L){
    L->active = L->first;
}

tInstructionItem* listInsert(tInstr *I){
    tInstructionItem *newItem;
    newItem = (tInstructionItem*) calloc(1, sizeof(tInstructionItem));
    if(newItem == NULL)
        return NULL;
    newItem->Instruction = I;
    newItem->next = NULL;
    if(ActualList->last == NULL) {
      //  printf("nastavuji first\n");
        ActualList->first = newItem;
        ActualList->last = newItem;
    }
    else {
       // printf("pridavam dal\n");
        ActualList->last->next = newItem;
        newItem->prev = ActualList->last;
        ActualList->last = newItem;
    }
    return newItem;
}
