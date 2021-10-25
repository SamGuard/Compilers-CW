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

Frame *allocFrame(Frame *prev) {
    Frame *f = (Frame *)malloc(sizeof(Frame));
    if (f == NULL) {
        perror("Cannot allocate memory");
        return NULL;
    }
    f->b = (Binding *)malloc(sizeof(Binding));
    if (f->b == NULL) {
        perror("Cannot allocate memory");
    }

    f->b->next = NULL;
    f->b->name = NULL;
    f->b->value = NULL;
    f->next = prev;
    return f;
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
    if (f == NULL) {
        perror("Variable not declared");
        return 0;
    }

    Binding *b = f->b;
    while (TRUE) {
        if (b == NULL) {
            return findBinding(ident, f->next);
        }
        if (b->name == ident) {
            return b;
        }
        b = b->next;
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
    if (prev != NULL) {
        prev->next = newBinding;
    }
}

void assign(TOKEN *ident, Value *x, Frame *f) {
    Binding *b = findBinding(ident, f);

    b->value = x;
}

Value *retrieve(TOKEN *ident, Frame *f) {
    Binding *b = findBinding(ident, f);
    if (b->value == NULL) {
        perror("variable not assigned");
    }

    return b->value;
}

// Takes pointer to the top of the AST containing params
// And a pointer to the new frame of vars
void applyArgsToParams(NODE *args, NODE *params, Frame *oldF, Frame *newF) {
    switch (params->type) {
        default:
            perror("Invalid token in parameter definition");
        case VOID:
            return;
        case ',':
            applyArgsToParams(args->left, params->left, oldF, newF);
            // applyArgsToParams(args->right, params->right, oldF, newF);
            args = args->right;
            params = params->right;
        case '~':;
            TOKEN *a = (TOKEN *)args->left;
            int type = ((TOKEN *)params->left->left)->type;
            Value *v;
            if (a->type == CONSTANT) {
                v = tokenToVal(a);
            } else if (a->type == IDENTIFIER) {
                v = retrieve(a, oldF);
            } else {
                perror("Invalid type in leaf");
                return;
            }

            declare((TOKEN *)params->right->left, v, newF);
    }
}

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
        case 0:
            return arithmetic(traverse(tree->left, f), NULL, 'N');
        case 'D': {
            Value *v = allocValue(FUNCTION, 0, NULL, tree);
            declare((TOKEN *)tree->left->right->left->left, v, f);
            return NULL;
        }
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

        case LEAF: {
            TOKEN *t = (TOKEN *)tree->left;
            if (t->type == CONSTANT) {
                return tokenToVal(t);
            } else if (t->type == IDENTIFIER) {
                return retrieve(t, f);
            } else {
                perror("Invalid type in leaf");
            }
        }
        case APPLY: {
            Frame *newFrame = allocFrame(f);
            NODE *func = retrieve((TOKEN *)tree->left->left, f)->f;
            applyArgsToParams(tree->right, func->left->right->right, f, newFrame);
            Value *v =
                traverse(func->right, newFrame);
            free(newFrame);
            return v;
        }
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

Value *callFunction(NODE *tree, Frame *f) {
    Frame *newFrame = allocFrame(f);
    Value *v = traverse(tree->right, newFrame);
    printFrame(newFrame);
    free(newFrame);
    return v;
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
    Frame *f = allocFrame(NULL);
    f->b = (Binding *)malloc(sizeof(Binding));
    f->next = NULL;
    traverse(tree, f);

    printf("Program exited with code: %d\n", callFunction(findMain(f), f)->i);
}
