#ifndef TAC_H
#define TAC_H
#include "./traverseStructures.h"

#define BRANCH 278
#define BRANCH_TRUE 279
#define BRANCH_FALSE 280
#define LABEL 281

#define SCOPE_UP 282
#define SCOPE_DOWN 283

#define DEFINE_FUNC_START 284
#define DEFINE_FUNC_END 285

#define SAVE_RET_ADDR 286
#define LOAD_RET_ADDR 287

#define SAVE_RET_VAL 288
#define LOAD_RET_VAL 289

#define DEC_ARG 290
#define DEF_PARAM 291

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