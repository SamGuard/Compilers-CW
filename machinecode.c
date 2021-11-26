#include "machinecode.h"

Binding *allocBinding(TOKEN *var, int memLoc) {
    Binding *newB = (Binding *)malloc(sizeof(Binding));
    if (newB == NULL) {
        perror("Cannot allocate memory in allocBinding");
    }
    newB->memLoc = memLoc;
    newB->var = var;
    newB->next = NULL;
}

void addInstruction(Block *b, int op, int arg1, int arg2, int arg3) {
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

void mathToInstruction(Block *b, int op, Tac* tac){
    int regA = REG_T_START, regB = REG_T_START + 1, regC = REG_T_START + 2;


    addInstruction(b, INS_LW, regA, getVarLocation(tac->src1, b->frame), 0);
    addInstruction(b, INS_LW, regB, getVarLocation(tac->src2,b->frame), 0);
    addInstruction(b, INS_ADD, regC, regA, regB);
    addInstruction(b, INS_SW, regC, getVarLocation(tac->dest, b->frame), 0);
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
            case '=': {
                addInstruction(block, INS_SW, tacList->src1->value,
                               getVarLocation(tacList->dest, block->frame), 0);
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

// Pritning goes here
void outputCode(Block *code) {
    while (code != NULL) {
        printf("----------\n");
        Inst *i = code->head;
        printf("addi $sp, $sp, %d\n", -4 * code->frame->frameSize);
        while (i != NULL) {
            switch (i->op) {
                default:
                    perror("Invalid instruction");
                    break;
                case INS_SW:
                    printf("sw $%d, %d($sp)\n", i->arg1, i->arg2);
                    break;
                case INS_LW:
                    printf("lw $%d, %d($sp)\n", i->arg1, i->arg2);
                    break;
                case INS_ADD:
                    printf("add $%d, $%d, $%d\n", i->arg1, i->arg2, i->arg3);
                    break;
            }
            i = i->next;
        }
        printf("addi $sp, $sp, %d\n", 4 * code->frame->frameSize);
        code = code->next;
    }
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