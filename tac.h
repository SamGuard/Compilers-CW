#ifndef TAC_H
#define TAC_H
#include "./traverseStructures.h"

void toTAC(NODE *tree);

typedef struct TAC {
    int op;
    TOKEN *src1;
    TOKEN *src2;
    TOKEN *dest;
    struct TAC *next;
} tac;
#endif