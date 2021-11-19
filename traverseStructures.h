#ifndef TRAVERSESTRUCTURES_H
#define TRAVERSESTRUCTURES_H
#include "./lexer_parser/nodes.h"
#include "./lexer_parser/token.h"
#include "./lexer_parser/C.tab.h"

#define BRANCH 278
#define BRANCH_TRUE 279
#define BRANCH_FALSE 280
#define LABEL 281

typedef struct Closure Closure;
typedef struct Frame Frame;
typedef struct Binding Binding;

typedef union Value {
    int i;    // Int
    char *s;  // String
    Closure *f;  // Function
} Value;

typedef struct Closure {
    NODE *f;
    Frame *prev;
} Closure;

typedef struct Binding {
    Value *value;
    TOKEN *name;
    struct Binding *next;
} Binding;

typedef struct Frame {
    Binding *b;
    struct Frame *next;
} Frame;

#endif