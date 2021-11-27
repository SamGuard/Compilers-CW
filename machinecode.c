#include "machinecode.h"

char CODE_START[] = ".text\n.globl	main\nmain:\n";
char CODE_END[] = "li $v0, 10\nsyscall\n";
const unsigned short WORD_SIZE = 4; // 4 bytes

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

void mathToInstruction(Block *b, int op, Tac *tac) {
    Number *regA = newNum(ADDR_REG, REG_T_START, NULL),
           *regB = newNum(ADDR_REG, REG_T_START + 1, NULL),
           *regC = newNum(ADDR_REG, REG_T_START + 2, NULL),
           *num1 = newNum(ADDR_BAS, getVarLocation(tac->src1, b->frame),
                          newNum(ADDR_REG, REG_SP, NULL)),
           *num2 = newNum(ADDR_BAS, getVarLocation(tac->src2, b->frame),
                          newNum(ADDR_REG, REG_SP, NULL)),
           *dest = newNum(ADDR_BAS, getVarLocation(tac->dest, b->frame),
                          newNum(ADDR_REG, REG_SP, NULL));

    addInstruction(b, INS_LW, regA, num1, NULL);
    addInstruction(b, INS_LW, regB, num2, NULL);
    addInstruction(b, INS_ADD, regC, regA, regB);
    addInstruction(b, INS_SW, regC, dest, NULL);
}

Block *traverseTac(BasicBlock *graph, Block *block) {
    Tac *tacList = graph->tac;
    while (tacList != NULL) {
        switch (tacList->op) {
            default:
                perror("Invalid operation");
                break;
            case 'H':
                break;
            case '~': {
                declare(tacList->dest, block->frame);
                break;
            }
            case '=': {  // Set a memory location to a value
                Number
                    // Hold the value 0
                    *zeroReg = newNum(ADDR_REG, 0, NULL),
                    // The register to put the value into
                    *destReg = newNum(ADDR_REG, REG_T_START, NULL),
                    *val = newNum(ADDR_IMM, tacList->src1->value, NULL),
                    *destMem = newNum(ADDR_BAS, getVarLocation(tacList->dest, block->frame),
                                        newNum(ADDR_REG, REG_SP, NULL));

                // Add zero and the value and store in register
                addInstruction(block, INS_ADD, destReg, zeroReg, val);
                // Write the value in the register to the memory
                addInstruction(block, INS_SW, destReg, destMem, NULL);
                break;
            }
            case '+': {
                mathToInstruction(block, '+', tacList);
            }
        }
        tacList = tacList->next;
    }
    return block;
}

void printNum(Number *n) {
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
}

void printInstruction(char *ins, Number *arg1, Number *arg2, Number *arg3) {
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
}

// Pritning goes here
void outputCode(Block *code) {
    printf(CODE_START);

    while (code != NULL) {
        Inst *i = code->head;
        printf("addi $sp, $sp, %d\n", -WORD_SIZE * code->frame->frameSize);
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
                case INS_ADD:
                    printInstruction("add", i->arg1, i->arg2, i->arg3);
                    break;
            }
            i = i->next;
        }
        printf("addi $sp, $sp, %d\n", WORD_SIZE * code->frame->frameSize);
        code = code->next;
    }
    printf(CODE_END);
}

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