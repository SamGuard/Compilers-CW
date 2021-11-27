#ifndef MACHINECODE_H
#define MACHINECODE_H
#include <stdio.h>
#include <stdlib.h>

#include "./tac.h"
#include "./traverseStructures.h"

#define INVALID 0 // Any instruction or addressing mode cannot be 0, helping catch mistakes

#define INS_LW 1
#define INS_SW 2
#define INS_ADD 3

#define REG_T_START 8 //Which register is the start of the temp registers
#define REG_SP 29 // Which register holds the stack pointer

#define ADDR_IMM 1 //Immediate addressing
#define ADDR_REG 2 //Register addressing
#define ADDR_BAS 3 //Base addressing


typedef struct Number {
    int addrMode;
    int value;
    struct Number *base;
} Number;

typedef struct Binding {
    TOKEN *var;
    int memLoc;
    struct Binding *next;
} Binding;

typedef struct Frame {
    Binding *b;
    unsigned int frameSize;
    struct Frame *next;
} Frame;

typedef struct Instruction {
    int op;
    Number *arg1, *arg2, *arg3;
    struct Instruction *next;
} Inst;

typedef struct Block {
    unsigned int memSize;
    Inst *head, *tail;
    Frame *frame;
    struct Block *next;
} Block;

void outputCode(Block *code);
void toMachineCode(BasicBlock *tree);
#endif