#ifndef TAC_H
#define TAC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./lexer_parser/nodes.h"
#include "./lexer_parser/token.h"
#include "./lexer_parser/C.tab.h"


#define BRANCH 278
#define BRANCH_TRUE 279
#define BRANCH_FALSE 280
#define LABEL 281

#define SCOPE_UP 282
#define SCOPE_DOWN 283
#define SCOPE_DOWN_FUNC 284 // Move scope down and mark that its a new chain
#define NEW_SCOPE 285 // New scope that links to global scope
#define RETURN_SCOPE 286 // When return is hit the scope needs to be reset


#define DEFINE_FUNC_START 287
#define DEFINE_FUNC_END 288

#define SAVE_RET_ADDR 289
#define LOAD_RET_ADDR 290

#define SAVE_RET_VAL 291
#define LOAD_RET_VAL 292

#define SAVE_MEM_POINT 293
#define LOAD_MEM_POINT 294

#define DEC_ARG 295
#define DEF_PARAM 296

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