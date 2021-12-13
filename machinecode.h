#ifndef MACHINECODE_H
#define MACHINECODE_H
#include <stdio.h>
#include <stdlib.h>

#include "./tac.h"
#include "./traverseStructures.h"

#define OUTPUT_MODE 1  // 0 is to print, 1 is write to file

#define WORD_SIZE 4  // Word size in bytes

#define MAX_ARG_SIZE 4; // Maximum amount of arguements a function can have

// Memory IO
#define INS_LW 512 // Load word
#define INS_LA 513 // Load address
#define INS_SW 514 // Store word

// Maths and Comparison
#define INS_ADD '+'
#define INS_SUB '-'
#define INS_LET '<'
#define INS_MUL '*'

//Branching
#define INS_BZE 515 // Branch if 0
#define INS_JMP 516 // Jump
#define INS_JAL 517 // Jump and set return address
#define INS_JPR 518 // Return from jump

// Psuedo instruction move stack pointer, frame size is only available
// post traversal so it set in outputCode
#define INS_SPD 519 // Decrease stack pointer
#define INS_SPU 520 // Increment stack pointer

#define INS_MFHI 521
#define INS_MFLO 522

#define REG_RET 2
#define REG_T_START 8  // Which register is the start of the temp registers
#define REG_SP 29      // Which register holds the stack pointer
#define REG_RA 31


#define ADDR_IMM 1  // Immediate addressing
#define ADDR_REG 2  // Register addressing
#define ADDR_BAS 3  // Base addressing
#define ADDR_IDT 4  // Identifier addressing
#define ADDR_LBL 5 // Used when number stores a label

typedef struct Frame Frame;

typedef struct Closure {
    unsigned int argSize;
    Tac* startLabel;
    Frame *prev;
} Closure;

typedef union Value {
    int i;    // Int
    char *s;  // String
    Closure *f;  // Function
} Value;

typedef struct Number {
    int addrMode;
    int value;
    int framesBack; // Counts the amount of frames back the variable was found
    struct Number *base;
} Number;

typedef struct Binding {
    TOKEN *var;
    Value memLoc;
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
    struct Block *next, *prev;
} Block;

void outputCode(Block *code);
void toMachineCode(BasicBlock *tree);
void setRegister(TOKEN *src, Block *b, Number *destReg);
void getArg(TOKEN *src, Block *b, Number *reg);
#endif