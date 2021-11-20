#include <stdio.h>
#include <stdlib.h>
#include "./tac.h"
#include "./traverseStructures.h"

#define INS_LW 1
#define INS_SW 2

typedef struct Binding {
    TOKEN *var;
    int memLoc;
    struct Binding *next;
} Binding;

typedef struct Frame {
    Binding *b;
    struct Frame *next;
} Frame;

typedef struct Instruction {
    int op;
    int arg1, arg2, arg3;
    struct Instruction *next;
} Inst;

typedef struct Block {
    unsigned int memSize;
    Inst *head, *tail;
    struct Block *next;
} Block;

void outputCode(Block *code);
void toMachineCode(Tac *tree);