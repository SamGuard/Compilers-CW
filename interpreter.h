#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "./traverseStructures.h"


int interpreter(NODE *tree);
Value *traverse(NODE*, Frame*);

#endif