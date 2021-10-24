#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>

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

TOKEN *allocToken(Value *value, int type, TOKEN *next) {
    TOKEN *x = (TOKEN *)malloc(sizeof(TOKEN));
    if (x == (void *)0) {
        perror("Cannot allocate memory");
    }
    x->type = type;
    x->lexeme = value->s;
    x->next = next;
    x->value = value->i;
    return x;
}

Value *allocValue(int type, int i, char *s, NODE *func) {
    Value *v = (Value *)malloc(sizeof(Value));
    if (v == (void *)0) {
        perror("Cannot allocate memory");
    }
    switch (type) {
        default:
            perror("Unknown type");
            return (void *)0;
        case INT:
            v->i = i;
            return v;
        case STRING_LITERAL:
            v->s = s;
            return v;
        case FUNCTION:
            v->f = func;
    }
}

Value *copyValue(Value *v) {
    Value *x = (Value *)malloc(sizeof(Value));
    x->f = v->f;
    x->i = v->i;
    x->s = v->s;
    return x;
}

Value *tokenToVal(TOKEN *t) {
    if (t->type == CONSTANT) {
        Value *v = allocValue(INT, t->value, NULL, NULL);
    }
}

Binding *findBinding(TOKEN *ident, Frame *f) {
    Binding *b = f->b;
    while (TRUE) {
        if (b->name == ident) {
            return b;
        }
        b = b->next;
        if (b == NULL) {
            perror("Could not find binding");
            return NULL;
        }
    }
}

void declare(TOKEN *ident, Value *value, Frame *f) {
    Binding *b = f->b;
    Binding *prev;
    while (b != NULL) {
        if (b->name == ident) {
            perror("variable has already been declared");
        }
        prev = b;
        b = b->next;
    }

    Binding *newBinding = (Binding *)malloc(sizeof(Binding));
    if (newBinding == NULL) {
        perror("Cannot allocate memory for new binding");
    }

    newBinding->value = value;
    newBinding->name = ident;
    newBinding->next = NULL;
    prev->next = newBinding;
}

void assign(TOKEN *ident, Value *x, Frame *f) {
    Binding *b = findBinding(ident, f);

    b->value = x;
}

Value *retrieve(TOKEN *ident, Frame *f) {
    if (f == NULL) {
        perror("Variable not declared");
        return 0;
    }

    Binding *b = f->b;
    while (TRUE) {
        if (b == NULL) {
            return retrieve(ident, f->next);
        }
        if (b->name == ident) {
            if (b->value == NULL) {
                perror("Variable regerenced before assignment");
            }
            return copyValue(b->value);
        }
        b = b->next;
    }
}

/*
Value *callFunction(NODE * , Frame *f) {
    Frame* newFrame = (Frame*)malloc(sizeof(Frame));

    traverse()

    free(newFrame);
}
*/

Value *arithmetic(Value *x, Value *y, char symbol) {
    Value *z = allocValue(INT, 0, NULL, NULL);

    switch (symbol) {
        default:
            perror("Invalid symbol");
        case '+':
            z->i = x->i + y->i;
            return z;
        case '-':
            z->i = x->i - y->i;
            return z;
        case '*':
            z->i = x->i * y->i;
            return z;
        case '/':
            z->i = x->i / y->i;
            return z;
        case 'N':  // Negate
            z->i = -x->i;
    }
}

Value *traverse(NODE *tree, Frame *f) {
    printf("%c\n", tree->type);
    switch (tree->type) {
        default:
            perror("unexpected type");
        case 0:;
            return arithmetic(traverse(tree->left, f), NULL, 'N');
        case 'D':;
            Value *v = allocValue(FUNCTION, 0, NULL, tree->right);
            declare((TOKEN *)tree->left->right->left->left, v, f);
            return NULL;
        case ';':
            if (tree->right != NULL) {
                traverse(tree->left, f);
                return traverse(tree->right, f);
            } else {
                return traverse(tree->left, f);
            }
        case '~':
            if (tree->left->type == LEAF) {
                if (tree->right->type == '=') {
                    declare((TOKEN *)tree->right->left->left, NULL, f);
                    traverse(tree->right, f);
                } else {
                    declare((TOKEN *)tree->right->left, NULL, f);
                }
                return NULL;
            } else {
                traverse(tree->left, f);
                traverse(tree->right, f);
                return NULL;
            }
        case '=':
            assign((TOKEN *)tree->left->left, traverse(tree->right, f), f);
            return NULL;
        case '+':
            return arithmetic(traverse(tree->left, f), traverse(tree->right, f),
                              '+');
        case '-':
            return arithmetic(traverse(tree->left, f), traverse(tree->right, f),
                              '-');
        case '*':
            return arithmetic(traverse(tree->left, f), traverse(tree->right, f),
                              '*');
        case '/':
            return arithmetic(traverse(tree->left, f), traverse(tree->right, f),
                              '/');

        case LEAF:;
            TOKEN *t = (TOKEN *)tree->left;
            if (t->type == CONSTANT) {
                return tokenToVal(t);
            } else if (t->type == IDENTIFIER) {
                return retrieve(t, f);
            } else {
                perror("Invalid type in leaf");
            }
        case APPLY:;
            Frame *newFrame = (Frame *)malloc(sizeof(Frame));
            newFrame->b = NULL;
            newFrame->next = f;

            //declareParameters(tree->right, )

            //free(newFrame);
            return traverse(retrieve((TOKEN *)tree->left->left, f)->f,
                            newFrame);
        case RETURN:
            return traverse(tree->left, f);
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
                printf("%d %s = %d, ", b->name->type, b->name->lexeme,
                       b->value->i);
            b = b->next;
        }
        f = f->next;
    }
}

NODE *findMain(Frame *f) {
    Binding *b = f->b;
    while (b != NULL) {
        if (b->name != NULL && strcmp(b->name->lexeme, "main") == 0) {
            return b->value->f;
        }
        b = b->next;
    }
}

void interpreter(NODE *tree) {
    Frame *f = (Frame *)malloc(sizeof(Frame));
    f->b = (Binding *)malloc(sizeof(Binding));
    f->next = NULL;
    traverse(tree, f);

    printf("Program exited with code: %d\n", traverse(findMain(f), f));
    // simplePrintTree(tree);
    printFrame(f);
}
