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

typedef struct BasicBlock{
    unsigned int size;
    Tac *tac, *tail;
    struct BasicBlock *next;
}BasicBlock;

BasicBlock *toTac(NODE *tree);
#endif