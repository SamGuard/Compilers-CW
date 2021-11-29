#ifndef MACHINECODE_H
#define MACHINECODE_H
#include <stdio.h>
#include <stdlib.h>
#include "./tac.h"
#include "./traverseStructures.h"

#define OUTPUT_MODE 1 // 0 is to print, 1 is write to file

#define WORD_SIZE 4 //Word size in bytes

#define INS_LW 257

#define INS_SW 258

#define INS_ADD '+'
#define INS_SUB '-'
#define INS_LET '<'

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