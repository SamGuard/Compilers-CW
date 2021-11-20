#ifndef TAC_H
#define TAC_H
#include "./traverseStructures.h"

typedef struct Tac {
    int op;
    TOKEN *src1;
    TOKEN *src2;
    TOKEN *dest;
    struct Tac *next;
} Tac;

Tac *toTac(NODE *tree);
#endif