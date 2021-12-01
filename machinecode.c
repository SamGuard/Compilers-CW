#include "machinecode.h"

const char CODE_START[] = ".text\n.globl	main\nmain:\n";
const char CODE_END[] = "li $v0, 10\nsyscall\n";
FILE *file;

Number *newNum(int addrMode, int value, Number *base) {
    Number *n = (Number *)malloc(sizeof(Number));
    if (n == NULL) {
        perror("Could allocate memory in newNum\n");
    }

    n->addrMode = addrMode;
    n->value = value;
    n->base = base;
    n->framesBack = -1;
}

Binding *allocBinding(TOKEN *var, int memLoc) {
    Binding *newB = (Binding *)malloc(sizeof(Binding));
    if (newB == NULL) {
        perror("Cannot allocate memory in allocBinding");
    }
    newB->memLoc = memLoc;
    newB->var = var;
    newB->next = NULL;
}

Frame *allocFrame() {
    Frame *f = (Frame *)malloc(sizeof(Frame));
    if (f == NULL) {
        perror("Cannot allocate memory in allocFrame");
    }
    f->frameSize = 0;
    f->next = NULL;
    f->b = NULL;
    return f;
}

Block *allocBlock() {
    Block *block = (Block *)malloc(sizeof(Block));
    if (block == NULL) {
        perror("Cannot allocate memory in allocBlock");
    }

    block->frame = allocFrame();
    block->head = block->tail = NULL;
    block->memSize = 0;
    block->next = NULL;
}

void addInstruction(Block *b, int op, Number *arg1, Number *arg2,
                    Number *arg3) {
    Inst *i;
    if (b->head == NULL) {
        i = b->head = b->tail = (Inst *)malloc(sizeof(Inst));
        if (i == NULL) {
            perror("Could not allocate memory in addInstruction");
        }
    } else {
        i = (Inst *)malloc(sizeof(Inst));
        ;
    }

    i->op = op;
    i->arg1 = arg1;
    i->arg2 = arg2;
    i->arg3 = arg3;
    b->tail->next = i;
    b->tail = i;
}

unsigned int declare(TOKEN *var, Frame *f) {
    if (f->b == NULL) {
        f->b = allocBinding(var, 0);
        f->frameSize++;
        return 0;
    }
    unsigned int count = 1;
    Binding *b = f->b;
    if (b->var == var) {
        perror("Variable already declared");
        return -1;
    }
    while (b->next != NULL) {
        if (b->var == var) {
            perror("Variable already declared");
            return -1;
        }
        b = b->next;
        count++;
    }
    f->frameSize++;
    b->next = allocBinding(var, count);
    return count;
}

Number *getVarLocation(TOKEN *var, Frame *f) {
    int found = FALSE;
    Binding *b = f->b;
    while (b != NULL) {
        if (b->var == var) {
            found = TRUE;
            break;
        }
        b = b->next;
    }

    if (found == TRUE) {
        Number *n = newNum(ADDR_BAS, b->memLoc, newNum(ADDR_REG, REG_SP, NULL));
        n->framesBack = 0;
        return n;
    }

    if (f->next == NULL) {
        perror("Variable not declared");
        return (void *)0;
    }
    Number *n = getVarLocation(var, f->next);
    n->framesBack++;
    return n;
}

// Returns a Number which is either immediate or register address
// Adds instruction to load value to register if needed
// Reg is the number to use to store the value in if needed
Number *getOperatorArg(TOKEN *src, Block *b, Number *reg) {
    Number *n;
    if (src->type == CONSTANT) {
        n = newNum(ADDR_IMM, src->value, NULL);
    } else if (src->type == IDENTIFIER) {
        n = reg;
        Number *memLocation = getVarLocation(src, b->frame);
        addInstruction(b, INS_LW, reg, memLocation, NULL);
    } else {
        printf("Invalid type in getOperatorArg\n");
    }
    return n;
}

void setRegister(TOKEN *src, Block *b, Number *destReg) {
    if (src->type == CONSTANT) {
        Number
            // Hold the value 0
            *zeroReg = newNum(ADDR_REG, 0, NULL),
            *val = newNum(ADDR_IMM, src->value, NULL);
        addInstruction(b, INS_ADD, destReg, zeroReg, val);
    } else if (src->type == IDENTIFIER) {
        getOperatorArg(src, b, destReg);
    } else {
        printf("Invalid type in setRegister %d\n", src->type);
    }
}

void mathToInstruction(Block *b, int op, Tac *tac) {
    // regA, regB are used to store the two argument IF necessary. They are
    // used to store values from memory, they are not used for immediate values
    // regC is the out register for the calculation
    // dest is the memory location to store the result in
    // num1, num2 stores either register pointer or immediate value
    Number *regA = newNum(ADDR_REG, REG_T_START, NULL),
           *regB = newNum(ADDR_REG, REG_T_START + 1, NULL),
           *regC = newNum(ADDR_REG, REG_T_START + 2, NULL),
           *dest = getVarLocation(tac->dest, b->frame),
           *num1 = getOperatorArg(tac->src1, b, regA),
           *num2 = getOperatorArg(tac->src2, b, regB);

    if ((op == '<' || op == EQ_OP) && num1->addrMode == ADDR_IMM) {
        num1 = regA;
        setRegister(tac->src1, b, regA);
    }

    addInstruction(b, op, regC, num1, num2);
    addInstruction(b, INS_SW, regC, dest, NULL);
}

Block *traverseTac(BasicBlock *graph, Block *block) {
    while (graph != NULL) {
        Tac *tacList = graph->tac;
        while (tacList != NULL) {
            switch (tacList->op) {
                default:
                    printf("Operator: %d\n", tacList->op);
                    perror("Invalid operation");
                    break;
                case 'H':
                    break;
                case '~': {
                    declare(tacList->dest, block->frame);
                    break;
                }
                case '=': {  // Set a memory location to a value

                    Number *destReg = newNum(ADDR_REG, REG_T_START, NULL),
                           *destMem =
                               getVarLocation(tacList->dest, block->frame);

                    // Load value into the register
                    setRegister(tacList->src1, block, destReg);
                    // Write the value in the register to the memory
                    addInstruction(block, INS_SW, destReg, destMem, NULL);
                    break;
                }
                case '+':
                case '-':
                case EQ_OP:
                case '<':
                    mathToInstruction(block, tacList->op, tacList);
                    break;

                case SCOPE_DOWN: {
                    Frame *newFrame = allocFrame();
                    newFrame->next = block->frame;
                    block->frame = newFrame;
                    addInstruction(block, INS_SPD, (Number *)newFrame, NULL,
                                   NULL);
                    break;
                }
                case SCOPE_UP: {
                    Frame *oldFrame = block->frame;
                    block->frame = block->frame->next;
                    addInstruction(block, INS_SPU, (Number *)oldFrame, NULL,
                                   NULL);
                    break;
                }
                case BRANCH:
                    addInstruction(block, BRANCH, (Number *)tacList->dest, NULL,
                                   NULL);
                    break;
                case BRANCH_FALSE: {
                    // Branch if the value is 0
                    // Register to load value into
                    Number *reg = newNum(ADDR_REG, REG_T_START, NULL);
                    Number *value = getOperatorArg(tacList->src1, block, reg);
                    addInstruction(block, INS_BZE, (Number *)tacList->dest,
                                   value, NULL);
                    break;
                }
                case LABEL:
                    addInstruction(block, LABEL, (Number *)tacList->dest, NULL,
                                   NULL);
                    break;
            }
            tacList = tacList->next;
        }
        graph = graph->next;
    }
    return block;
}

// ------------------------------PRINTING-------------------

// Returns the amount bytes to move back to find a variable in a different frame
int calcVariableOffset(Frame *f, Number *n) {
    Frame *currFrame = f;
    int stepsBack = n->framesBack;
    if (stepsBack == -1 || n->addrMode != ADDR_BAS) {
        perror("Invlaid number given to calcVariableOffset");
    }

    unsigned int count = n->value;
    while (stepsBack > 0) {
        count += currFrame->frameSize;
        currFrame = currFrame->next;
        stepsBack--;
        if (currFrame == NULL) {
            perror("No more frames to look through, bad variable");
        }
    }
    return count * WORD_SIZE;
}

void printNum(Frame *f, Number *n) {
#if OUTPUT_MODE == 0
    switch (n->addrMode) {
        case ADDR_REG:
            printf("$");
            printf("%d", n->value);
            break;
        case ADDR_IMM:
            printf("%d", n->value);
            break;
        case ADDR_BAS:
            printf("%d(", calcVariableOffset(f, n));
            printNum(f, n->base);
            printf(")");
            break;
    }
#endif
#if OUTPUT_MODE == 1
    switch (n->addrMode) {
        case ADDR_REG:
            fprintf(file, "$");
            fprintf(file, "%d", n->value);
            break;
        case ADDR_IMM:
            fprintf(file, "%d", n->value);
            break;
        case ADDR_BAS:
            fprintf(file, "%d(", calcVariableOffset(f, n));
            printNum(f, n->base);
            fprintf(file, ")");
            break;
    }
#endif
}

void printLabel(Number *label) {
    TOKEN *l = (TOKEN *)label;
#if OUTPUT_MODE == 0
    printf("%s%d:\n", l->lexeme, l->value);
#endif

#if OUTPUT_MODE == 1
    fprintf(file, "%s%d:\n", l->lexeme, l->value);
#endif
}

void printBranch(Frame *f, Number *label, Number *val) {
    TOKEN *l = (TOKEN *)label;
    if (val == NULL) {
#if OUTPUT_MODE == 0
        printf("j %s%d\n", l->lexeme, l->value);
#endif
#if OUTPUT_MODE == 1
        fprintf(file, "j %s%d\n", l->lexeme, l->value);
#endif
        return;
    }

#if OUTPUT_MODE == 0
    printf("beqz %s%d, ", l->lexeme, l->value);
    printNum(f, val);
    printf("\n");
#endif

#if OUTPUT_MODE == 1
    fprintf(file, "beqz ");
    printNum(f, val);
    fprintf(file, ", %s%d", l->lexeme, l->value);
    fprintf(file, "\n");
#endif
}

void printMoveStack(int bytesToMove) {
#if OUTPUT_MODE == 0
    printf("addi $sp, $sp, %d\n", bytesToMove);
#endif
#if OUTPUT_MODE == 1
    fprintf(file, "addi $sp, $sp, %d\n", bytesToMove);
#endif
}

void printInstruction(char *ins, Frame *f, Number *arg1, Number *arg2,
                      Number *arg3) {
#if OUTPUT_MODE == 0
    printf("%s ", ins);
    printNum(f, arg1);
    if (arg2 == NULL) {
        printf("\n");
        return;
    }
    printf(", ");
    printNum(f, arg2);
    if (arg3 == NULL) {
        printf("\n");
        return;
    }
    printf(", ");
    printNum(f, arg3);
    printf("\n");
#endif

#if OUTPUT_MODE == 1
    fprintf(file, "%s ", ins);
    printNum(f, arg1);
    fprintf(file, ", ");
    printNum(f, arg2);
    if (arg3 == NULL) {
        fprintf(file, "\n");
        return;
    }
    fprintf(file, ", ");
    printNum(f, arg3);
    fprintf(file, "\n");
#endif
}

void printIndent(unsigned int depth) {
    for (int i = 0; i < depth; i++) {
#if OUTPUT_MODE == 0
        printf("    ");
#endif
#if OUTPUT_MODE == 1
        fprintf(file, "   ");
#endif
    }
}

// Pritning goes here
void outputCode(Block *code) {
    unsigned int depth = 0;
#if OUTPUT_MODE == 0
    printf("\n%s", CODE_START);
#endif
#if OUTPUT_MODE == 1
    file = fopen("./outputs/out.asm", "w");
    fprintf(file, CODE_START);
#endif

    while (code != NULL) {
#if OUTPUT_MODE == 0
        printf("addi $sp, $sp, %d\n", -1 * WORD_SIZE * code->frame->frameSize);
#endif
#if OUTPUT_MODE == 1
        fprintf(file, "addi $sp, $sp, %d\n",
                -1 * WORD_SIZE * code->frame->frameSize);
#endif

        Frame *currFrame = code->frame;
        Inst *i = code->head;
        while (i != NULL) {
            printIndent(depth);
            switch (i->op) {
                default:
                    perror("Invalid instruction");
                    break;
                case INS_SW:
                    printInstruction("sw", currFrame, i->arg1, i->arg2, NULL);
                    break;
                case INS_LW:
                    printInstruction("lw", currFrame, i->arg1, i->arg2, NULL);
                    break;
                case '+':
                    printInstruction("add", currFrame, i->arg1, i->arg2,
                                     i->arg3);
                    break;
                case '-':
                    printInstruction("sub", currFrame, i->arg1, i->arg2,
                                     i->arg3);
                    break;
                case EQ_OP:
                    printInstruction("seq", currFrame, i->arg1, i->arg2,
                                     i->arg3);
                    break;
                case '<':
                    printInstruction("slt", currFrame, i->arg1, i->arg2,
                                     i->arg3);
                    break;
                case LABEL:
                    printLabel(i->arg1);
                    break;
                case INS_SPD:
                    depth++;
                    currFrame = (Frame *)i->arg1;
                    printMoveStack(currFrame->frameSize * WORD_SIZE * -1);
                    break;
                case INS_SPU:
                    if (depth == 0)
                        perror("No more stack to pop");
                    else
                        depth--;
                    printMoveStack(currFrame->frameSize * WORD_SIZE);
                    currFrame = currFrame->next;
                    break;
                case BRANCH:
                case INS_BZE:
                    printBranch(currFrame, i->arg1, i->arg2);
                    break;
            }
            i = i->next;
        }
#if OUTPUT_MODE == 0
        printf("addi $sp, $sp, %d\n", code->frame->frameSize);
#endif
#if OUTPUT_MODE == 1
        fprintf(file, "addi $sp, $sp, %d\n", code->frame->frameSize);
#endif
        code = code->next;
    }
#if OUTPUT_MODE == 0
    printf(CODE_END);
#endif
#if OUTPUT_MODE == 1
    fprintf(file, CODE_END);
    fclose(file);
#endif
}

// ------------------------------PRINTING-------------------

void toMachineCode(BasicBlock *tree) {
    Block b;
    Frame f;

    f.b = NULL;
    f.frameSize = 0;
    f.next = NULL;
    b.head = b.tail = NULL;
    b.next = NULL;
    b.memSize = 0;
    b.frame = &f;

    outputCode(traverseTac(tree, &b));
}