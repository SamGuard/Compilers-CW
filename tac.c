#include "Tac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERBOSE

int tempCounter = 0;
int labelCounter = 0;
TOKEN *traverse(NODE *tree, BasicBlock *block);

void moveToFrontTac(Tac **t) {
    while ((*t)->next != NULL) {
        *t = (*t)->next;
    }
}

void appendTac(Tac **dest, Tac *src) {
    if (*dest == NULL) {
        perror("Cannot append tac to null list");
        return;
    }
    moveToFrontTac(dest);
    (*dest)->next = src;
}

void moveToFrontBlock(BasicBlock **dest) {
    while ((*dest)->next != NULL) {
        (*dest) = (*dest)->next;
    }
}

void appendBlock(BasicBlock **dest, BasicBlock *src) {
    moveToFrontBlock(dest);
    (*dest)->next = src;
}

Tac *allocTac(TOKEN *token) {
    Tac *t = (Tac *)malloc(sizeof(Tac));
    if (t == NULL) {
        perror("cannot allocate memory in newTac\n");
    }
    t->next = NULL;
    t->dest = token;
    return t;
}

Tac *allocTemp(Tac **tac) {
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

    Tac *declareIns = (Tac *)malloc(sizeof(Tac));
    declareIns->next = NULL;
    declareIns->dest = newTok;
    declareIns->op = '~';

    appendTac(tac, declareIns);
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

BasicBlock *allocBasicBlock() {
    BasicBlock *newBasicBlock = (BasicBlock *)malloc(sizeof(BasicBlock));
    Tac *homeTac = (Tac *)malloc(sizeof(Tac));
    if (newBasicBlock == NULL || homeTac == NULL) {
        perror("cannot allocate memory in allocBasicBlock");
        return NULL;
    }

    homeTac->op = 'H';
    homeTac->dest = homeTac->src1 = homeTac->src2 = NULL;
    homeTac->next = NULL;

    newBasicBlock->next = NULL;
    newBasicBlock->size = 0;
    newBasicBlock->tac = newBasicBlock->tail = homeTac;
    return newBasicBlock;
}

Tac *arithmetic(NODE *tree, BasicBlock *block, int op) {
    Tac *t = allocTemp(&block->tail);
    t->op = op;
    t->src1 = traverse(tree->left, block);
    t->src2 = traverse(tree->right, block);
    appendTac(&block->tail, t);
    return t;
}

TOKEN *traverse(NODE *tree, BasicBlock *block) {
    Tac **prev = &block->tail;  // Previous tac to append onto
#ifdef VERBOSE
    printf("%c\n", tree->type);
#endif
    switch (tree->type) {
        default:
            perror("unexpected type");
        case 0:
            break;
        case 'D':
            traverse(tree->right, block);
            return 0;
        case ';':
            traverse(tree->left, block);
            moveToFrontBlock(&block);
            traverse(tree->right, block);
            return 0;
        case '~': {
            Tac *t;
            if (tree->right->type == '=') {
                t = allocTac((TOKEN *)tree->right->left->left);
            } else {
                t = allocTac((TOKEN *)tree->right->left);
            }
            t->op = '~';
            appendTac(prev, t);
            traverse(tree->right, block);
            return 0;
        }
        case '=': {
            Tac *t = allocTac((TOKEN *)tree->left->left);
            t->op = '=';
            t->src1 = traverse(tree->right, block);
            appendTac(prev, t);
            return t->dest;
        }
        case '+': {
            Tac *t = arithmetic(tree, block, tree->type);
            return t->dest;
        }

        break;
        case '-': {
            Tac *t = arithmetic(tree, block, tree->type);
            return t->dest;
        } break;
        case '*': {
            Tac *t = arithmetic(tree, block, tree->type);
            return t->dest;
        }
        case '/': {
            Tac *t = arithmetic(tree, block, tree->type);
            return t->dest;
        }
        case '%': {
            Tac *t = arithmetic(tree, block, tree->type);
            return t->dest;
        }
        case '<': {
            Tac *t = arithmetic(tree, block, tree->type);
            return t->dest;
        }
        case '>': {
            Tac *t = allocTemp(prev);
            t->op = LE_OP;
            t->src2 = traverse(tree->left, block);
            t->src1 = traverse(tree->right, block);
            appendTac(prev, t);
            return t->dest;
        }
        case EQ_OP: {
            Tac *t = arithmetic(tree, block, tree->type);
            return t->dest;
        }

        case NE_OP: {
            Tac *t = arithmetic(tree, block, tree->type);
            return t->dest;
        }
        case LE_OP: {
            Tac *t = arithmetic(tree, block, tree->type);
            return t->dest;
        }
        case GE_OP: {
            Tac *t = arithmetic(tree, block, tree->type);
            return t->dest;
        }
        case IF: {
            TOKEN *tok = traverse(tree->left, block);

            Tac *branch = allocTac(NULL);
            Tac *endIfLabel = allocLabel();

            // Branch instruction, label to jump to is set depending on whether
            // or not its an if-else statement
            branch->op = BRANCH_FALSE;
            branch->src1 = tok;
            appendTac(prev, branch);

            // Allocate block for if body
            BasicBlock *ifBlock = allocBasicBlock();
            BasicBlock *postIfBlock = allocBasicBlock();

            block->next = ifBlock;

            if (tree->right->type == ELSE) {
                // Alloc block for else body
                BasicBlock *elseBlock = allocBasicBlock();
                ifBlock->next = elseBlock;
                elseBlock->next = postIfBlock;

                Tac *elseLabel = allocLabel();
                branch->dest = elseLabel->dest;

                traverse(tree->right->left, ifBlock);
                Tac *endIfBranch = allocTac(NULL);
                endIfBranch->op = BRANCH;
                endIfBranch->dest = endIfLabel->dest;
                appendTac(&ifBlock->tac, endIfBranch);

                appendTac(&elseBlock->tac, elseLabel);
                traverse(tree->right->right, elseBlock);

            } else {
                ifBlock->next = postIfBlock;
                branch->dest = endIfLabel->dest;
                traverse(tree->right, ifBlock);
            }
            appendTac(&(postIfBlock->tac), endIfLabel);
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

void printTac(BasicBlock *block) {
    while (block != NULL) {
        printf("-----NEW-BLOCK-----\n");
        Tac *t = block->tac;
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
            } else if (t->op == '~') {
                if (t->dest->lexeme[0] == '_') {
                    printf("~ %s%d", t->dest->lexeme, t->dest->value);
                } else {
                    printf("~ %s", t->dest->lexeme);
                }
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
        block = block->next;
    }
}

BasicBlock *toTac(NODE *tree) {
    BasicBlock *head = allocBasicBlock();

    traverse(tree, head);
#ifdef VERBOSE
    printTac(head);
#endif
    return head;
}