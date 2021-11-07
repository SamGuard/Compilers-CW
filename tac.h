#ifndef TAC_H
#define TAC_H
#include "./traverseStructures.h"

#define BRANCH 278
#define BRANCH_TRUE 279
#define BRANCH_FALSE 280
#define LABEL 281

void toTac(NODE *tree);

typedef struct TAC {
    int op;
    TOKEN *src1;
    TOKEN *src2;
    TOKEN *dest;
    struct TAC *next;
} tac;
#endif