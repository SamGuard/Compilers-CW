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

void appendTac(BasicBlock *dest, Tac *src) {
    if (dest == NULL) {
        perror("Cannot append tac to null list");
        return;
    }
    moveToFrontTac(&dest->tail);
    (dest)->tail->next = src;
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

Tac *allocTemp(BasicBlock *block) {
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

    appendTac(block, declareIns);
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

void moveScope(BasicBlock *block, int moveUp) {
    if (moveUp == TRUE) {
        Tac *scopeUp = allocTac(NULL);
        scopeUp->op = SCOPE_UP;
        scopeUp->next = NULL;
        appendTac(block, scopeUp);
    } else {
        Tac *scopeDown = allocTac(NULL);
        scopeDown->op = SCOPE_DOWN;
        scopeDown->next = NULL;
        appendTac(block, scopeDown);
    }
}

Tac *arithmetic(NODE *tree, BasicBlock *block, int op) {
    Tac *t = allocTemp(block);
    t->op = op;
    t->src1 = traverse(tree->left, block);
    t->src2 = traverse(tree->right, block);
    appendTac(block, t);
    return t;
}

TOKEN *traverse(NODE *tree, BasicBlock *block) {
    Tac **prev = &block->tail;  // Previous tac to append onto
    switch (tree->type) {
        default:
            perror("unexpected type");
        case 0:
            break;
        case 'D': {
            BasicBlock *funcBlock = allocBasicBlock();
            Tac *funcStart = allocLabel(), *funcDefStart = allocTac(NULL),
                *funcDefEnd = allocTac(NULL);            
            funcDefStart->op = DEFINE_FUNC_START;
            // Function name
            funcDefStart->dest = (TOKEN *)tree->left->right->left->left;
            // Label for the function
            funcDefStart->src1 = (TOKEN *)funcStart;
            funcStart->dest->lexeme = funcDefStart->dest->lexeme; 

            funcDefEnd->op = DEFINE_FUNC_END;

            appendBlock(&block, funcBlock);

            appendTac(funcBlock, funcDefStart);
            appendTac(funcBlock, funcStart);
            moveScope(funcBlock, FALSE);
            traverse(tree->right, funcBlock);
            moveToFrontBlock(&funcBlock);
            appendTac(funcBlock, funcDefEnd);
            break;
        }
        case ';':
            traverse(tree->left, block);
            moveToFrontBlock(&block);
            traverse(tree->right, block);
            return 0;
        case '~': {
            if (tree->left->type == LEAF) {
                Tac *t;
                if (tree->right->type == '=') {
                    t = allocTac((TOKEN *)tree->right->left->left);
                } else {
                    t = allocTac((TOKEN *)tree->right->left);
                }
                t->op = '~';
                appendTac(block, t);
                traverse(tree->right, block);
                return 0;
            } else {
                traverse(tree->left, block);
                moveToFrontBlock(&block);
                traverse(tree->right, block);
                return 0;
            }
        }
        case '=': {
            Tac *t = allocTac((TOKEN *)tree->left->left);
            t->op = '=';
            t->src1 = traverse(tree->right, block);
            appendTac(block, t);
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
            Tac *t = allocTemp(block);
            t->op = LE_OP;
            t->src2 = traverse(tree->left, block);
            t->src1 = traverse(tree->right, block);
            appendTac(block, t);
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
            TOKEN *condition = traverse(tree->left, block);

            Tac *branch = allocTac(NULL);
            Tac *endIfLabel = allocLabel();

            // Branch instruction, label to jump to is set depending on whether
            // or not its an if-else statement
            branch->op = BRANCH_FALSE;
            branch->src1 = condition;
            appendTac(block, branch);

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

                moveScope(ifBlock, FALSE);
                traverse(tree->right->left, ifBlock);
                moveToFrontBlock(&ifBlock);
                moveScope(ifBlock, TRUE);

                Tac *endIfBranch = allocTac(NULL);
                endIfBranch->op = BRANCH;
                endIfBranch->dest = endIfLabel->dest;
                appendTac(ifBlock, endIfBranch);

                appendTac(elseBlock, elseLabel);
                moveScope(elseBlock, FALSE);
                traverse(tree->right->right, elseBlock);
                moveToFrontBlock(&elseBlock);
                moveScope(elseBlock, TRUE);

            } else {
                ifBlock->next = postIfBlock;
                branch->dest = endIfLabel->dest;
                moveScope(ifBlock, FALSE);
                traverse(tree->right, ifBlock);
                moveToFrontBlock(&ifBlock);
                moveScope(ifBlock, TRUE);
            }
            appendTac(postIfBlock, endIfLabel);
        }
        case WHILE: {
            BasicBlock *preWhileBlock = allocBasicBlock(),
                       *whileBlock = allocBasicBlock(),
                       *postWhileBlock = allocBasicBlock();
            Tac *loopStartLabel = allocLabel(), *loopEndLabel = allocLabel(),
                *branchIfFalse = allocTac(NULL),
                *branchToCondition = allocTac(NULL);

            branchIfFalse->op = BRANCH_FALSE;
            branchIfFalse->dest = loopEndLabel->dest;

            branchToCondition->op = BRANCH;
            branchToCondition->dest = loopStartLabel->dest;

            appendBlock(&block, preWhileBlock);

            /* Structure of the loop
                - block 0
                Move scope down
                Start label:
                Calculate condition
                If false then branch to end label
                - block 1
                Body of the while
                branch back to begining
                - block 2
                end label:
                Move scope up
            */
            moveScope(preWhileBlock, FALSE);
            appendTac(preWhileBlock, loopStartLabel);
            TOKEN *condition = traverse(tree->left, preWhileBlock);
            appendTac(preWhileBlock, branchIfFalse);
            traverse(tree->right, whileBlock);
            appendTac(postWhileBlock, branchToCondition);
            appendTac(postWhileBlock, loopEndLabel);
            moveScope(postWhileBlock, TRUE);

            appendBlock(&preWhileBlock, whileBlock);
            appendBlock(&whileBlock, postWhileBlock);

            branchIfFalse->src1 =
                condition;  // Set the pointer to the condition code
            break;
        }
        case LEAF: {
            return (TOKEN *)tree->left;
        }
        case APPLY: {
            BasicBlock *postCallBlock = allocBasicBlock();
            TOKEN *retAddrName = (TOKEN *)malloc(sizeof(TOKEN));
            if (retAddrName == NULL)
                perror("Could not allocate memory in APPLY");
            retAddrName->lexeme = (char *)"RA";
            retAddrName->type = IDENTIFIER;

            Tac *callFunc = allocTac(NULL), *decVar = allocTac(NULL),
                *saveAddr = allocTac(NULL), *loadAddr = allocTac(NULL);

            decVar->op = '~';
            decVar->dest = retAddrName;

            saveAddr->op = SAVE_RET_ADDR;
            saveAddr->dest = retAddrName;

            loadAddr->op = LOAD_RET_ADDR;
            loadAddr->dest = retAddrName;

            callFunc->op = APPLY;
            callFunc->dest = (TOKEN *)tree->left->left;

            // Structure:
            // Declare return address
            // Save return address
            // Call function
            // Load return address back into register
            appendTac(block, decVar);
            appendTac(block, saveAddr);
            moveScope(block, FALSE);
            appendTac(block, callFunc);
            appendBlock(&block, postCallBlock);
            moveScope(postCallBlock, TRUE);
            appendTac(postCallBlock, loadAddr);
        }
        case BREAK:
            break;
        case RETURN: {
            moveScope(block, TRUE);
            Tac *retTac = allocTac(NULL);
            retTac->op = RETURN;
            appendTac(block, retTac);

        } break;
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
    int depth = 0;
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
            } else if (t->op == SAVE_RET_ADDR) {
                printf("---SAVE RET ADDR---");
            } else if (t->op == LOAD_RET_ADDR) {
                printf("---LOAD RET ADDR---");
            } else if (t->op == RETURN) {
                printf("return");
            } else if (t->op == DEFINE_FUNC_START) {
                printf("----START FUNC DEFINITION----");
            } else if (t->op == DEFINE_FUNC_END) {
                printf("----END FUNC DEFINITION----");
            } else if (t->op == APPLY) {
                printf("Call %s", t->dest->lexeme);
            } else if (t->op == '~') {
                if (t->dest->lexeme[0] == '_') {
                    printf("~ %s%d", t->dest->lexeme, t->dest->value);
                } else {
                    printf("~ %s", t->dest->lexeme);
                }
            } else if (t->op == SCOPE_DOWN) {
                depth++;
                printf("---SCOPE_DOWN---");
            } else if (t->op == SCOPE_UP) {
                depth--;
                printf("----SCOPE_UP----");
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