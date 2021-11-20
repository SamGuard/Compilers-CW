#include "Tac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tempCounter = 0;
int labelCounter = 0;
TOKEN *traverse(NODE *tree, Tac *prev);

Tac *allocTac(TOKEN *token) {
    Tac *t = (Tac *)malloc(sizeof(Tac));
    if (t == NULL) {
        perror("cannot allocate memory in newTac\n");
    }
    t->next = NULL;
    t->dest = token;
    return t;
}

Tac *allocTemp() {
    Tac *newTac = (Tac *)malloc(sizeof(Tac));
    TOKEN *newTok = (TOKEN *)malloc(sizeof(TOKEN));
    if (newTac == NULL || newTok == NULL) {
        perror("cannot allocate memory in newTac\n");
    }

    newTok->lexeme = "_T";
    newTok->type = IDENTIFIER;
    newTok->value = tempCounter;

    newTac->dest = newTok;
    newTac->next = NULL;

    tempCounter++;
    return newTac;
}

Tac *allocLabel() {
    Tac *newTac = (Tac *)malloc(sizeof(Tac));
    TOKEN *newTok = (TOKEN *)malloc(sizeof(TOKEN));
    if (newTac == NULL || newTok == NULL) {
        perror("cannot allocate memory in newTac\n");
    }

    newTok->lexeme = "_L";
    newTok->type = IDENTIFIER;
    newTok->value = labelCounter;

    newTac->dest = newTok;
    newTac->next = NULL;
    newTac->op = LABEL;

    labelCounter++;
    return newTac;
}

void moveToFront(Tac **t) {
    while ((*t)->next != NULL) {
        *t = (*t)->next;
    }
}

void appendTac(Tac **prev, Tac *t) {
    moveToFront(prev);
    (*prev)->next = t;
}

Tac *arithmetic(NODE *tree, Tac *prev, int op) {
    Tac *t = allocTemp();
    t->op = op;
    t->src1 = traverse(tree->left, prev);
    t->src2 = traverse(tree->right, prev);
    appendTac(&prev, t);
    return t;
}

TOKEN *traverse(NODE *tree, Tac *prev) {
    printf("%c\n", tree->type);
    switch (tree->type) {
        default:
            perror("unexpected type");
        case 0:
            break;
        case 'D':
            traverse(tree->right, prev);
            return 0;
        case ';':
            traverse(tree->left, prev);
            traverse(tree->right, prev);
            return 0;
        case '~': {
            Tac *t = allocTac((TOKEN *)tree->right->left->left);
            t->op = '~';
            appendTac(&prev, t);
            traverse(tree->right, prev);
            return 0;
        }
        case '=': {
            Tac *t = allocTac((TOKEN *)tree->left->left);
            t->op = '=';
            t->src1 = traverse(tree->right, prev);
            appendTac(&prev, t);
            return t->dest;
        }
        case '+': {
            Tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }

        break;
        case '-': {
            Tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        } break;
        case '*': {
            Tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case '/': {
            Tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case '%': {
            Tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case '<': {
            Tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case '>': {
            Tac *t = allocTemp();
            t->op = LE_OP;
            t->src2 = traverse(tree->left, prev);
            t->src1 = traverse(tree->right, prev);
            appendTac(&prev, t);
            return t->dest;
        }
        case EQ_OP: {
            Tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }

        case NE_OP: {
            Tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case LE_OP: {
            Tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case GE_OP: {
            Tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case IF: {
            TOKEN *tok = traverse(tree->left, prev);

            Tac *branch = allocTac(NULL);
            Tac *endIfLabel = allocLabel();

            branch->op = BRANCH_FALSE;
            branch->src1 = tok;
            appendTac(&prev, branch);

            if (tree->right->type == ELSE) {
                Tac *elseLabel = allocLabel();
                branch->dest = elseLabel->dest;

                traverse(tree->right->left, prev);
                Tac *endIfBranch = allocTac(NULL);
                endIfBranch->op = BRANCH;
                endIfBranch->dest = endIfLabel->dest;
                appendTac(&prev, endIfBranch);

                appendTac(&prev, elseLabel);
                traverse(tree->right->right, prev);
            } else {
                branch->dest = endIfLabel->dest;
                traverse(tree->right, prev);
            }
            appendTac(&prev, endIfLabel);
        }
        case WHILE:
            break;
        case LEAF: {
            return (TOKEN *)tree->left;
        }
        case APPLY:
            break;
        case BREAK:
            break;
        case RETURN:
            break;
    }
}

void printOP(int op) {
    if (op < 256) {
        printf("%c", op);
        return;
    }
    switch (op) {
        case EQ_OP:
            printf("==");
            return;
        case NE_OP:
            printf("!=");
            return;
        case LE_OP:
            printf("<=");
            return;
        case GE_OP:
            printf(">=");
            return;
        case BRANCH:
            return;
        case BRANCH_TRUE:
            printf("IF");
            return;
        case BRANCH_FALSE:
            printf("IF NOT");
            return;
    }
}

void printToken(TOKEN *t) {
    if (t->type == CONSTANT) {
        printf("%d", t->value);
        return;
    }
    if (t->lexeme[0] == '_') {
        printf("_%c%d", t->lexeme[1], t->value);
        return;
    }
    printf("%s", t->lexeme);
}

void printTac(Tac *t) {
    while (t != NULL) {
        if (t->op == 'H') {
            t = t->next;
            continue;
        }
        if (t->op == BRANCH) {
            printf("BRANCH ");
            printToken(t->dest);
        } else if (t->op == BRANCH_FALSE || t->op == BRANCH_TRUE) {
            printf("BRANCH ");
            printOP(t->op);
            printf("(");
            printToken(t->src1);
            printf(")");
            printf("->");
            printToken(t->dest);
        } else if (t->op == LABEL) {
            printToken(t->dest);
            printf(":");
        } else if (t->op == '~'){
            printf("~ %s", t->dest->lexeme);
        } else {
            printToken(t->dest);
            printf("=");
            printToken(t->src1);
            if (t->op != '=' && t->src2 != NULL) {
                printOP(t->op);
                printToken(t->src2);
            }
        }
        printf("\n");
        t = t->next;
    }
}

Tac *toTac(NODE *tree) {
    Tac *head = (Tac *)malloc(sizeof(Tac));
    head->op = 'H';
    head->dest = head->src1 = head->src2 = NULL;
    head->next = NULL;
    traverse(tree, head);
    printTac(head);
    return head;
}