#ifndef TAC_H
#define TAC_H
#include "./traverseStructures.h"

typedef struct TAC {
    int op;
    TOKEN *src1;
    TOKEN *src2;
    TOKEN *dest;
    struct TAC *next;
} tac;

tac *toTac(NODE *tree);
#endif