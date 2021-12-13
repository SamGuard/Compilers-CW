#ifndef TAC_H
#define TAC_H
#include "./traverseStructures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BRANCH 278
#define BRANCH_TRUE 279
#define BRANCH_FALSE 280
#define LABEL 281

#define SCOPE_UP 282
#define SCOPE_DOWN 283
#define NEW_SCOPE 284 // New scope that links to global scope
#define RETURN_SCOPE 285 // When return is hit the scope needs to be reset


#define DEFINE_FUNC_START 286
#define DEFINE_FUNC_END 287

#define SAVE_RET_ADDR 288
#define LOAD_RET_ADDR 289

#define SAVE_RET_VAL 290
#define LOAD_RET_VAL 291

#define SAVE_MEM_POINT 292
#define LOAD_MEM_POINT 293

#define DEC_ARG 294
#define DEF_PARAM 295

typedef struct Tac {
    int op;
    TOKEN *src1;
    TOKEN *src2;
    TOKEN *dest;
    struct Tac *next;
} Tac;

typedef struct BasicBlock {
    unsigned int size;
    Tac *tac, *tail;
    struct BasicBlock *next;
} BasicBlock;

void declareArgs(NODE *tree, BasicBlock *block, int argNum);
BasicBlock *toTac(NODE *tree);
#endif