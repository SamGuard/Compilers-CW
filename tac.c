#include "Tac.h"

#define VERBOSE

int tempCounter = 0;
int labelCounter = 0;
int functionCounter = 0;
int functionDepth = 0;  // 0 if global scope, incremented each time entering
                        // function, decremented when leaving
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
    dest->tail->next = src;
    dest->tail = dest->tail->next;
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
    moveToFrontBlock(&block);
    t->src2 = traverse(tree->right, block);
    moveToFrontBlock(&block);
    appendTac(block, t);
    return t;
}

void declareArgs(NODE *tree, BasicBlock *block, int argNum) {
    if (tree == NULL) {
        return;
    }
    switch (tree->type) {
        case VOID:
            return;
        case ',':
            declareArgs(tree->left, block, argNum + 1);
            moveToFrontBlock(&block);
            tree = tree->right;
        default: {
            TOKEN *dest = (TOKEN *)malloc(sizeof(TOKEN));
            if (dest == NULL) perror("Could not allocate in declare args");
            dest->lexeme = NULL;
            dest->type = CONSTANT;
            dest->value = argNum;
            Tac *arg = allocTac(dest);
            arg->op = DEC_ARG;
            arg->src1 = traverse(tree, block);
            moveToFrontBlock(&block);
            appendTac(block, arg);
            break;
        }
    }
}

void defineParams(NODE *tree, BasicBlock *block, int argNum) {
    if (tree == NULL) return;
    printf("%c\n", tree->type);
    switch (tree->type) {
        case VOID:
            return;
        case ',':
            defineParams(tree->left, block, argNum + 1);
            moveToFrontBlock(&block);
            tree = tree->right;
        case '~': {
            TOKEN *dest = (TOKEN *)malloc(sizeof(TOKEN));
            if (dest == NULL) perror("Could not allocate in declare args");
            dest->lexeme = NULL;
            dest->type = CONSTANT;
            dest->value = argNum;
            Tac *arg = allocTac(dest);
            arg->op = DEF_PARAM;
            arg->src1 = (TOKEN *)tree->right->left;
            appendTac(block, arg);
            break;
        }
    }
}

TOKEN *traverse(NODE *tree, BasicBlock *block) {
    moveToFrontBlock(&block);
    // printf("%c\n", tree->type);
    switch (tree->type) {
        default:
            perror("unexpected type");
        case 0:
            break;
        case 'D': {
            BasicBlock *funcBlock = allocBasicBlock();
            Tac *funcDefStart = allocTac(NULL), *funcStartLabel = allocLabel(),
                *funcMoveScope = allocTac(NULL),
                *funcResetScope = allocTac(NULL), *funcDefEnd = allocTac(NULL),
                *funcEndLabel = allocLabel();

            funcDefStart->op = DEFINE_FUNC_START;
            // Function name
            funcDefStart->dest = (TOKEN *)tree->left->right->left->left;
            if (strcmp("main", funcDefStart->dest->lexeme) == 0) {
                funcDefStart->dest->value = 0;
            } else {
                functionCounter += 1;
                funcDefStart->dest->value = functionCounter;
            }
            // Label for the function
            funcDefStart->src1 = (TOKEN *)funcStartLabel;
            funcStartLabel->dest->lexeme = funcDefStart->dest->lexeme;
            funcStartLabel->dest->value = -1;

            // Used to skip the function if its being defined, not called
            funcDefStart->src2 = funcEndLabel->dest;

            funcResetScope->op = RETURN_SCOPE;
            funcDefEnd->op = DEFINE_FUNC_END;

            appendBlock(&block, funcBlock);

            appendTac(funcBlock, funcDefStart);
            appendTac(funcBlock, funcStartLabel);
            // New scope if defined in global scope or move down and signal to
            // create new chain of frames linked to the previous set
            if (functionDepth == 0) {
                funcMoveScope->op = NEW_SCOPE;
            } else {
                funcMoveScope->op = SCOPE_DOWN_FUNC;
            }
            appendTac(funcBlock, funcMoveScope);
            functionDepth++;
            defineParams(tree->left->right->right, funcBlock, 0);
            moveToFrontBlock(&funcBlock);
            traverse(tree->right, funcBlock);
            moveToFrontBlock(&funcBlock);
            if (funcBlock->tail->op != RETURN) {
                appendTac(funcBlock, funcResetScope);
            }
            functionDepth--;
            appendTac(funcBlock, funcDefEnd);
            appendTac(funcBlock, funcEndLabel);
            break;
        }
        case ';':
            if (tree->right != NULL) {
                if (tree->left != NULL) traverse(tree->left, block);
                moveToFrontBlock(&block);
                traverse(tree->right, block);
                return 0;
            } else {
                traverse(tree->left, block);
                moveToFrontBlock(&block);
                return 0;
            }

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
            moveToFrontBlock(&block);
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
            moveToFrontBlock(&block);
            t->src1 = traverse(tree->right, block);
            moveToFrontBlock(&block);
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

                Tac *elseLabel = allocLabel();
                branch->dest = elseLabel->dest;

                moveScope(ifBlock, FALSE);
                traverse(tree->right->left, ifBlock);
                appendBlock(&ifBlock, elseBlock);
                moveScope(ifBlock, TRUE);

                Tac *endIfBranch = allocTac(NULL);
                endIfBranch->op = BRANCH;
                endIfBranch->dest = endIfLabel->dest;
                appendTac(ifBlock, endIfBranch);

                appendTac(elseBlock, elseLabel);
                moveScope(elseBlock, FALSE);
                traverse(tree->right->right, elseBlock);
                appendBlock(&elseBlock, postIfBlock);
                moveScope(elseBlock, TRUE);

            } else {
                branch->dest = endIfLabel->dest;
                moveScope(ifBlock, FALSE);
                traverse(tree->right, ifBlock);
                moveToFrontBlock(&ifBlock);
                moveScope(ifBlock, TRUE);
                ifBlock->next = postIfBlock;
            }
            appendTac(postIfBlock, endIfLabel);
            break;
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
            appendBlock(&preWhileBlock, whileBlock);
            traverse(tree->right, whileBlock);
            appendBlock(&whileBlock, postWhileBlock);
            appendTac(postWhileBlock, branchToCondition);
            appendTac(postWhileBlock, loopEndLabel);
            moveScope(postWhileBlock, TRUE);

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
            TOKEN *memPointName = (TOKEN *)malloc(sizeof(TOKEN));
            if (retAddrName == NULL || memPointName == NULL)
                perror("Could not allocate memory in APPLY");
            retAddrName->lexeme = (char *)"RA";
            retAddrName->type = IDENTIFIER;

            memPointName->lexeme = (char *)"MP";
            memPointName->type = IDENTIFIER;

            Tac *callFunc = allocTac(NULL), *decRA = allocTac(NULL),
                *decMP = allocTac(NULL), *saveAddr = allocTac(NULL),
                *saveMP = allocTac(NULL), *loadAddr = allocTac(NULL),
                *loadMP = allocTac(NULL), *loadVal = allocTac(NULL),
                *returnValue = allocTemp(block);

            decRA->op = '~';
            decRA->dest = retAddrName;

            decMP->op = '~';
            decMP->dest = memPointName;

            saveAddr->op = SAVE_RET_ADDR;
            saveAddr->dest = retAddrName;

            loadAddr->op = LOAD_RET_ADDR;
            loadAddr->dest = retAddrName;

            saveMP->op = SAVE_MEM_POINT;
            saveMP->dest = memPointName;

            loadMP->op = LOAD_MEM_POINT;
            loadMP->dest = memPointName;

            loadVal->op = LOAD_RET_VAL;
            loadVal->dest = returnValue->dest;

            callFunc->op = APPLY;
            callFunc->dest = (TOKEN *)tree->left->left;

            // Structure:
            // Declare return value
            // Declare return address
            // Save return address
            // Call function
            // Load return value
            // Load return address back into register
            moveToFrontBlock(&block);
            appendTac(block, decRA);
            appendTac(block, decMP);
            appendTac(block, saveAddr);
            appendTac(block, saveMP);
            moveScope(block, FALSE);
            declareArgs(tree->right, block, 0);
            moveToFrontBlock(&block);
            appendTac(block, callFunc);
            appendBlock(&block, postCallBlock);
            appendTac(postCallBlock, loadVal);
            moveScope(postCallBlock, TRUE);
            appendTac(postCallBlock, loadAddr);
            appendTac(postCallBlock, loadMP);
            moveToFrontBlock(&block);
            return returnValue->dest;
        }
        case BREAK:
            break;
        case RETURN: {
            Tac *storeReturnVal = allocTac(NULL), *resetScope = allocTac(NULL);
            storeReturnVal->op = SAVE_RET_VAL;
            storeReturnVal->dest = traverse(tree->left, block);

            resetScope->op = RETURN_SCOPE;
            moveToFrontBlock(&block);
            appendTac(block, storeReturnVal);
            appendTac(block, resetScope);
            Tac *retTac = allocTac(NULL);
            retTac->op = RETURN;
            appendTac(block, retTac);
            break;
        }
    }
    return NULL;
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
            switch (t->op) {
                case 'H':
                    t = t->next;
                    continue;
                case BRANCH:
                    printf("BRANCH ");
                    printToken(t->dest);
                    break;
                case BRANCH_FALSE:
                case BRANCH_TRUE:
                    printf("BRANCH ");
                    printOP(t->op);
                    printf("(");
                    printToken(t->src1);
                    printf(")");
                    printf("->");
                    printToken(t->dest);
                    break;
                case LABEL:
                    printToken(t->dest);
                    printf(":");
                    break;
                case SAVE_RET_ADDR:
                    printf("---SAVE RET ADDR---");
                    break;
                case LOAD_RET_ADDR:
                    printf("---LOAD RET ADDR---");
                    break;
                case SAVE_MEM_POINT:
                    printf("---SAVE MEM POINT---");
                    break;
                case LOAD_MEM_POINT:
                    printf("---LOAD MEM POINT---");
                    break;
                case SAVE_RET_VAL:
                    printf("RETURN VALUE = ");
                    printToken(t->dest);
                    break;
                case LOAD_RET_VAL:
                    printToken(t->dest);
                    printf(" = RETURN VALUE");
                    break;
                case RETURN:
                    printf("return");
                    break;
                case DEFINE_FUNC_START:
                    printf("----START FUNC DEFINITION----");
                    break;
                case DEFINE_FUNC_END:
                    printf("----END FUNC DEFINITION----");
                    break;
                case DEC_ARG:
                    printf("~ ARG %d ", t->dest->value);
                    printToken(t->src1);
                    break;
                case DEF_PARAM:
                    printf("~ PARAM %d ", t->dest->value);
                    printToken(t->src1);
                    break;
                case APPLY:
                    printf("Call %s", t->dest->lexeme);
                    break;
                case '~':
                    if (t->dest->lexeme[0] == '_') {
                        printf("~ %s%d", t->dest->lexeme, t->dest->value);
                    } else {
                        printf("~ %s", t->dest->lexeme);
                    }
                    break;
                case SCOPE_DOWN_FUNC:
                    printf("---SCOPE_DOWN_FUNC---");
                    break;
                case SCOPE_DOWN:
                    printf("---SCOPE_DOWN---");
                    break;
                case NEW_SCOPE:
                    printf("---NEW_SCOPE---");
                    break;
                case SCOPE_UP:
                    printf("----SCOPE_UP----");
                    break;
                case RETURN_SCOPE:
                    printf("----RETURN_SCOPE----");
                    break;
                default:
                    printToken(t->dest);
                    printf("=");
                    printToken(t->src1);
                    if (t->op != '=' && t->src2 != NULL) {
                        printOP(t->op);
                        printToken(t->src2);
                    }
                    break;
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