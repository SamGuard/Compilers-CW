#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "./traverseStructures.h"

typedef struct Closure Closure;
typedef struct Frame Frame;
typedef struct Binding Binding;


typedef struct Closure {
    NODE *f;
    Frame *prev;
} Closure;

typedef union Value {
    int i;    // Int
    char *s;  // String
    Closure *f;  // Function
} Value;

typedef struct Binding {
    Value *value;
    TOKEN *name;
    struct Binding *next;
} Binding;

typedef struct Frame {
    Binding *b;
    struct Frame *next;
} Frame;

int interpreter(NODE *tree);
Value *traverse(NODE*, Frame*);

#endif