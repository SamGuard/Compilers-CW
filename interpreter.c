#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Frame *globalFrame;
int returning, breaking;

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
            declare((TOKEN *)params->right->left, traverse(args, oldF), newF);
    }
}

Value *arithmetic(Value *x, Value *y, char symbol) {
    Value *z = allocValue(INT, 0, NULL, NULL);

    switch (symbol) {
        default:
            perror("Invalid symbol");
        case '+':
            z->i = x->i + y->i;
            break;
        case '-':
            z->i = x->i - y->i;
            break;
        case '*':
            z->i = x->i * y->i;
            break;
        case '/':
            z->i = x->i / y->i;
            break;
        case 'N':  // Negate
            z->i = -x->i;
        case '%':
            z->i = x->i % y->i;
            break;
    }
    free(x);
    free(y);
    return z;
}

// x, y are the values to be compared, symbol is the comparison and tree is a
// pointer to the if node
Value *logic(Value *x, Value *y, int symbol) {
    Value *z = allocValue(INT, FALSE, NULL, NULL);
    switch (symbol) {
    defualt:
        perror("Invalid logical operation");
        case '=':
            if (x->i == y->i) {
                z->i = TRUE;
            }
            return z;
        case '!':
            if (x->i != y->i) {
                z->i = TRUE;
            }
            return z;
        case ',':
            if (x->i <= y->i) {
                z->i = TRUE;
            }
            return z;
        case '.':
            if (x->i >= y->i) {
                z->i = TRUE;
            }
        case '<':
            if (x->i < y->i) {
                z->i = TRUE;
            }
            return z;
        case '>':
            if (x->i > y->i) {
                z->i = TRUE;
            }
            return z;
    }
}

Value *traverse(NODE *tree, Frame *f) {
    //printf("%c\n", tree->type);
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
                Value *lResult, *rResult;

                if (tree->left != NULL) lResult = traverse(tree->left, f);
                if (returning || breaking) return lResult;
                rResult = traverse(tree->right, f);
                if (tree->left != NULL && tree->left->type == RETURN) {
                    return lResult;
                }
                return rResult;
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
        case '%':
            return arithmetic(traverse(tree->left, f), traverse(tree->right, f),
                              '%');
        case '<':
            return logic(traverse(tree->left, f), traverse(tree->right, f),
                         '<');
        case '>':
            return logic(traverse(tree->left, f), traverse(tree->right, f),
                         '>');
        case EQ_OP:
            return logic(traverse(tree->left, f), traverse(tree->right, f),
                         '=');
        case NE_OP:
            return logic(traverse(tree->left, f), traverse(tree->right, f),
                         '!');
        case LE_OP:
            return logic(traverse(tree->left, f), traverse(tree->right, f),
                         ',');
        case GE_OP:
            return logic(traverse(tree->left, f), traverse(tree->right, f),
                         '.');

        case IF: {
            Value *res = traverse(tree->left, f);
            if (res->i == FALSE) {
                if (tree->right->type == ELSE) {
                    Frame *newFrame = allocFrame(f);
                    Value *res = traverse(tree->right->right, newFrame);
                    free(newFrame);
                    f->next = NULL;
                    return res;
                }
                return NULL;
            } else {
                Frame *newFrame = allocFrame(f);
                Value *res;
                if (tree->right->type == ELSE) {
                    res = traverse(tree->right->left, newFrame);
                } else {
                    res = traverse(tree->right, newFrame);
                }
                free(newFrame);
                f->next = NULL;
                return res;
            }
        }

        case WHILE: {
            Frame *newFrame = allocFrame(f);
            Value *res;
            while (traverse(tree->left, newFrame)->i == TRUE) {
                res = traverse(tree->right, newFrame);
                if (breaking || returning) {
                    breaking = FALSE;
                    return res;
                }
                free(newFrame);
                newFrame = allocFrame(f);
            }
            return NULL;
        }

        case LEAF: {
            TOKEN *t = (TOKEN *)tree->left;
            if (t->type == CONSTANT) {
                return tokenToVal(t);
            } else if (t->type == IDENTIFIER) {
                return copyValue(retrieve(t, f));
            } else {
                perror("Invalid type in leaf");
            }
        }
        case APPLY: {
            Frame *newFrame = allocFrame(globalFrame);
            NODE *func = retrieve((TOKEN *)tree->left->left, f)->f;
            applyArgsToParams(tree->right, func->left->right->right, f,
                              newFrame);
            Value *v = traverse(func->right, newFrame);
            returning = FALSE;
            breaking = FALSE;
            free(newFrame);
            return v;
        }
        case BREAK:
            breaking = TRUE;
            return NULL;
        case RETURN:{
            Value *v = traverse(tree->left, f);
            returning = TRUE;
            return v;
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
                printf("%d %s = %d, ", b->name->type, b->name->lexeme,
                       b->value->i);
            b = b->next;
        }
        f = f->next;
    }
}

Value *callMain(NODE *tree, Frame *f) {
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

int interpreter(NODE *tree) {
    globalFrame = allocFrame(NULL);
    globalFrame->b = (Binding *)malloc(sizeof(Binding));
    globalFrame->next = NULL;
    returning = 0; breaking = 0;
    traverse(tree, globalFrame);
    int result = callMain(findMain(globalFrame), globalFrame)->i;
    printf("Program exited with code: %d\n", result);
    free(globalFrame);
    return result;
}
