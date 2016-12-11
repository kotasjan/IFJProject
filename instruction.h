#pragma once


typedef enum
{
   I_PUSH_GLOBAL,
    I_TEST,
    I_ASSIGNMENT,
    I_MUL,
    I_DIV,
    I_ADD,
    I_MINUS,
    I_ASSIGN_EXPR,
    I_SET_START,
    I_PUSH,
    I_MINUS_FIRST,
    I_MINUS_SECOND,
    I_MINUS_BOTH,
    I_ADD_BOTH,
    I_ADD_FIRST,
    I_ADD_SECOND,
    I_DIV_FIRST,
    I_DIV_SECOND,
    I_MUL_BOTH,
    I_MUL_FIRST,
    I_DIV_BOTH,
    I_MUL_SECOND,
    I_IF,
    I_LESS,
    I_GREATER,
    I_LESS_EQUAL,
    I_GREATER_EQUAL,
    I_NOT_EQUAL,
    I_EQUAL,
    I_LESS_FIRST,
    I_GREATER_FIRST,
    I_LESS_EQUAL_FIRST,
    I_GREATER_EQUAL_FIRST,
    I_NOT_EQUAL_FIRST,
    I_EQUAL_FIRST,
    I_LESS_SECOND,
    I_GREATER_SECOND,
    I_LESS_EQUAL_SECOND,
    I_GREATER_EQUAL_SECOND,
    I_NOT_EQUAL_SECOND,
    I_EQUAL_SECOND = 60,
    I_LESS_BOTH,
    I_GREATER_BOTH,
    I_LESS_EQUAL_BOTH,
    I_GREATER_EQUAL_BOTH,
    I_NOT_EQUAL_BOTH,
    I_EQUAL_BOTH,
    I_SKIP,
    I_WHILE,
    I_SKIP_NEXT_INST,
    I_SKIP_BEFORE_WHILE,
    I_SKIP_TAPE,
    I_CALL,
    I_SET_CLASS,
    I_PARAM,
    I_RETURN,
    I_SET_RET,
    I_PRINT,
    I_FUNC_CMP,
    I_SET_CONCAT,
    I_CONCAT,

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

extern tInstructionList *ActualList;