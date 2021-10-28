#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "./lexer_parser/nodes.h"
#include "./lexer_parser/C.tab.h"
#include "./lexer_parser/token.h"

typedef union Value {
    int i;    // Int
    char *s;  // String
    NODE *f;  // Function
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