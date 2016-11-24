#pragma once

typedef enum
{
   I_PUSH_GLOBAL,
 I_TEST,
 I_ASSIGNMENT,
} tInstrType;

typedef struct _tInstr {
    tInstrType InstrType;
    void *addr1;
    void *addr2;
    void *addr3;
} tInstr;


typedef struct _tInstructionItem {
    tInstr *Instruction;
    struct _tInstructionItem *next;
    struct _tInstructionItem *prev;
} tInstructionItem;


typedef  struct {
    tInstructionItem *first;
    tInstructionItem *active;
    tInstructionItem *last;
} tInstructionList;

tInstructionList *ActualList;