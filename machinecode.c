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

int getVarLocation(TOKEN *var, Frame *f) {
    Binding *b = f->b;
    while (b != NULL) {
        if (b->var == var) {
            return b->memLoc;
        }
        b = b->next;
    }
    if (f->next == NULL) {
        perror("Variable not declared");
        return -1;
    }
    getVarLocation(var, f->next);
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
        Number *memLocation = newNum(ADDR_BAS, getVarLocation(src, b->frame),
                                     newNum(ADDR_REG, REG_SP, NULL));
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
           *dest = newNum(ADDR_BAS, getVarLocation(tac->dest, b->frame),
                          newNum(ADDR_REG, REG_SP, NULL)),
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
                           newNum(ADDR_BAS,
                                  getVarLocation(tacList->dest, block->frame),
                                  newNum(ADDR_REG, REG_SP, NULL));

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

            case BRANCH_FALSE: {
            }
        }
        tacList = tacList->next;
    }
    return block;
}

// ------------------------------PRINTING-------------------

void printNum(Number *n) {
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
            printf("%d(", WORD_SIZE * n->value);
            printNum(n->base);
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
            fprintf(file, "%d(", WORD_SIZE * n->value);
            printNum(n->base);
            fprintf(file, ")");
            break;
    }
#endif
}

void printInstruction(char *ins, Number *arg1, Number *arg2, Number *arg3) {
#if OUTPUT_MODE == 0
    printf("%s ", ins);
    printNum(arg1);
    printf(", ");
    printNum(arg2);
    if (arg3 == NULL) {
        printf("\n");
        return;
    }
    printf(", ");
    printNum(arg3);
    printf("\n");
#endif

#if OUTPUT_MODE == 1
    fprintf(file, "%s ", ins);
    printNum(arg1);
    fprintf(file, ", ");
    printNum(arg2);
    if (arg3 == NULL) {
        fprintf(file, "\n");
        return;
    }
    fprintf(file, ", ");
    printNum(arg3);
    fprintf(file, "\n");
#endif
}

// Pritning goes here
void outputCode(Block *code) {
#if OUTPUT_MODE == 0
    printf("\n%s", CODE_START);
#endif
#if OUTPUT_MODE == 1
    file = fopen("./outputs/out.asm", "w");
    fprintf(file, CODE_START);
#endif

    while (code != NULL) {
        Inst *i = code->head;
#if OUTPUT_MODE == 0
        printf("addi $sp, $sp, %d\n", -WORD_SIZE * code->frame->frameSize);
#endif
#if OUTPUT_MODE == 1
        fprintf(file, "addi $sp, $sp, %d\n",
                -WORD_SIZE * code->frame->frameSize);
#endif
        while (i != NULL) {
            switch (i->op) {
                default:
                    perror("Invalid instruction");
                    break;
                case INS_SW:
                    printInstruction("sw", i->arg1, i->arg2, NULL);
                    break;
                case INS_LW:
                    printInstruction("lw", i->arg1, i->arg2, NULL);
                    break;
                case '+':
                    printInstruction("add", i->arg1, i->arg2, i->arg3);
                    break;
                case '-':
                    printInstruction("sub", i->arg1, i->arg2, i->arg3);
                    break;
                case EQ_OP:
                    printInstruction("seq", i->arg1, i->arg2, i->arg3);
                    break;
                case '<':
                    printInstruction("slt", i->arg1, i->arg2, i->arg3);
                    break;
            }
            i = i->next;
        }
#if OUTPUT_MODE == 0
        printf("addi $sp, $sp, %d\n", WORD_SIZE * code->frame->frameSize);
#endif
#if OUTPUT_MODE == 1
        fprintf(file, "addi $sp, $sp, %d\n",
                WORD_SIZE * code->frame->frameSize);
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