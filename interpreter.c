#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct Binding {
    int value;
    TOKEN *name;
    struct Binding *next;
} Binding;

typedef struct Frame {
    Binding *b;
    struct Frame *next;
} Frame;

TOKEN *allocToken(int value, int type, char *lexeme, TOKEN *next) {
    TOKEN *x = (TOKEN *)malloc(sizeof(TOKEN));
    if (x == NULL) {
        perror("Cannot allocate memory");
    }
    x->type = type;
    x->lexeme = lexeme;
    x->next = next;
    x->value = value;
    return x;
}

void declare(TOKEN *ident, int value, Frame *f) {
    Binding *b = f->b;
    Binding *prev;
    while (b != NULL) {
        if (b->name == ident) {
            perror("variable has already been declared");
            exit(-1);
        }
        prev = b;
        b = b->next;
    }

    Binding *newBinding = (Binding *)malloc(sizeof(Binding));
    if (newBinding == NULL) {
        perror("Cannot allocate memory for new binding");
        exit(-1);
    }
    newBinding->value = value;
    newBinding->name = ident;
    newBinding->next = NULL;
    prev->next = newBinding;
}

TOKEN *retrieve(TOKEN *ident, Frame *f) {
    Binding *b = f->b;
    while (TRUE) {
        if (b->name == ident) {
            return allocToken(b->value, CONSTANT, NULL, NULL);
        }
        if (b->next == NULL) {
            perror("Variable regerenced before assignment");
        }
        b = b->next;
    }
}

void assign(TOKEN *x, Frame *f) {}

TOKEN *arithmetic(TOKEN *x, TOKEN *y, char symbol) {
    if (x->type != CONSTANT || y->type != CONSTANT) {
        perror("Cannot perform arithmatic on non-constants");
    }

    TOKEN *z = allocToken(0, CONSTANT, NULL, NULL);

    switch (symbol) {
        default:
            perror("Invalid symbol");
        case '+':
            z->value = x->value + y->value;
            return z;
    }
}

TOKEN *traverse(NODE *tree, Frame *f) {
    printf("%c\n", tree->type);
    switch (tree->type) {
        default:
            perror("unexpected type");
        case 'D':
            return traverse(tree->right, f);
        case ';':
            traverse(tree->left, f);
            if (tree->right != NULL) traverse(tree->right, f);
            return NULL;
        case '~':
            traverse(tree->right, f);
            return NULL;
        case '=':
            declare((TOKEN *)tree->left->left, traverse(tree->right, f)->value,
                    f);
            return NULL;
        case '+':
            return arithmetic(traverse(tree->left, f), traverse(tree->right, f),
                              '+');
        case LEAF:;
            TOKEN *t = (TOKEN *)tree->left;
            if (t->type == CONSTANT) {
                return t;
            } else if (t->type == IDENTIFIER) {
                return retrieve(t, f);
            }
    }
}

void simplePrintTree(NODE *tree) {
    NODE *head = tree;
    if (head->type == LEAF) {
        TOKEN *t = ((TOKEN *)head->left);
        if (t->type == IDENTIFIER) {
            printf("Leaf value: %s\n", t->lexeme);
        } else {
            printf("Leaf value: %d\n", t->value);
        }
        return;
    }
    printf("type: %c  \n", head->type);
    interpreter(head->left);
    interpreter(head->right);
}

void printFrame(Frame *f) {
    while (f != NULL) {
        Binding *b = f->b;
        while (b != NULL) {
            if (b->name != NULL)
                printf("%d %s = %d, ", b->name->type, b->name->lexeme, b->value);
            b = b->next;
        }
        f = f->next;
    }
}

void interpreter(NODE *tree) {
    Frame *f = (Frame *)malloc(sizeof(Frame));
    f->b = (Binding *)malloc(sizeof(Binding));
    f->next = NULL;
    traverse(tree, f);
    // simplePrintTree(tree);
    printFrame(f);
}
