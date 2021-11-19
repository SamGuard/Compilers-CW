#include "tac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tempCounter = 0;
int labelCounter = 0;
TOKEN *traverse(NODE *tree, tac *prev);

tac *allocTac(TOKEN *token) {
    tac *t = (tac *)malloc(sizeof(tac));
    if (t == NULL) {
        perror("cannot allocate memory in newTac\n");
    }
    t->next = NULL;
    t->dest = token;
    return t;
}

tac *allocTemp() {
    tac *newTac = (tac *)malloc(sizeof(tac));
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

tac *allocLabel() {
    tac *newTac = (tac *)malloc(sizeof(tac));
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

void moveToFront(tac **t) {
    while ((*t)->next != NULL) {
        *t = (*t)->next;
    }
}

void appendTac(tac **prev, tac *t) {
    moveToFront(prev);
    (*prev)->next = t;
}

tac *arithmetic(NODE *tree, tac *prev, int op) {
    tac *t = allocTemp();
    t->op = op;
    t->src1 = traverse(tree->left, prev);
    t->src2 = traverse(tree->right, prev);
    appendTac(&prev, t);
    return t;
}

TOKEN *traverse(NODE *tree, tac *prev) {
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
        case '~':
            traverse(tree->right, prev);
            return 0;
        case '=': {
            tac *t = allocTac((TOKEN *)tree->left->left);
            t->op = '=';
            t->src1 = traverse(tree->right, prev);
            appendTac(&prev, t);
            return t->dest;
        }
        case '+': {
            tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }

        break;
        case '-': {
            tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        } break;
        case '*': {
            tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case '/': {
            tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case '%': {
            tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case '<': {
            tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case '>': {
            tac *t = allocTemp();
            t->op = LE_OP;
            t->src2 = traverse(tree->left, prev);
            t->src1 = traverse(tree->right, prev);
            appendTac(&prev, t);
            return t->dest;
        }
        case EQ_OP: {
            tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }

        case NE_OP: {
            tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case LE_OP: {
            tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case GE_OP: {
            tac *t = arithmetic(tree, prev, tree->type);
            return t->dest;
        }
        case IF: {
            TOKEN *tok = traverse(tree->left, prev);

            tac *branch = allocTac(NULL);
            tac *endIfLabel = allocLabel();

            branch->op = BRANCH_FALSE;
            branch->src1 = tok;
            appendTac(&prev, branch);

            if (tree->right->type == ELSE) {
                tac *elseLabel = allocLabel();
                branch->dest = elseLabel->dest;

                traverse(tree->right->left, prev);
                tac *endIfBranch = allocTac(NULL);
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
    if(op < 256){
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

void printTac(tac *t) {
    while (t != NULL) {
        if (t->op == 'H') {
            t = t->next;
            continue;
        }
        if(t->op == BRANCH){
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

tac* toTac(NODE *tree) {
    tac* head = (tac*)malloc(sizeof(tac));
    head->op = 'H';
    head->dest = head->src1 = head->src2 = NULL;
    head->next = NULL;
    traverse(tree, head);
    printTac(head);
    return head;
}