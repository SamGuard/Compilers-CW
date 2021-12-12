#include "machinecode.h"

const char CODE_DATA[] =
    ".data\n"
    "args: .space 128 # Allocate 128 bytes for arguemnts\n"
    "func_def: .space 1024 # Allocate 1024 bytes for functions\n";
const char CODE_START[] =
    ".text\n"
    ".globl	main\n"
    "main:\n";

const char CODE_CALL_MAIN[] = 
    "jal main0\n"
    "add $4, $2, $0\n"
    "li $v0, 1\n"
    "syscall\n"
    "li $v0, 10\n"
    "syscall\n";

char* CODE_FUNC_DEF;

const char *ARGS_IDENT = "args";
FILE *file;  // File to write assembly to

Frame *globalFrame;

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

Binding *allocBinding(TOKEN *var, Value memLoc) {
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
    return block;
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
    }

    i->op = op;
    i->arg1 = arg1;
    i->arg2 = arg2;
    i->arg3 = arg3;
    b->tail->next = i;
    b->tail = i;
}

Value declare(TOKEN *var, Frame *f) {
    if (f->b == NULL) {
        Value z;
        z.i = 0;
        f->b = allocBinding(var, z);
        f->frameSize++;
        return z;
    }
    Value count;
    count.i = 1;
    Binding *b = f->b;
    if (b->var == var) {
        perror("Variable already declared");
        Value z;
        z.i = -1;
        return z;
    }
    while (b->next != NULL) {
        if (b->var == var) {
            perror("Variable already declared");
            Value z;
            z.i = -1;
            return z;
        }
        b = b->next;
        count.i++;
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
        Number *n =
            newNum(ADDR_BAS, b->memLoc.i, newNum(ADDR_REG, REG_SP, NULL));
        n->framesBack = 0;
        return n;
    }

    if (f->next == NULL) {
        printf("Variable not declared: %s\n", var->lexeme);
        return newNum(ADDR_IMM, 0, NULL);
    }
    Number *n = getVarLocation(var, f->next);
    n->framesBack++;
    return n;
}

// Returns a Number which is either immediate or register address
// Adds instruction to load value to register if needed
// Reg is the number to use to store the value in if needed
void getArg(TOKEN *src, Block *b, Number *reg) {
    Number *n;
    if (src->type == CONSTANT) {
        setRegister(src, b, reg);
        n = reg;
    } else if (src->type == IDENTIFIER) {
        n = reg;
        Number *memLocation = getVarLocation(src, b->frame);
        addInstruction(b, INS_LW, reg, memLocation, NULL);
    } else {
        printf("Invalid type in getArg\n");
    }
}

void setRegister(TOKEN *src, Block *b, Number *destReg) {
    if (src->type == CONSTANT) {
        Number
            // Hold the value 0
            *zeroReg = newNum(ADDR_REG, 0, NULL),
            *val = newNum(ADDR_IMM, src->value, NULL);
        addInstruction(b, INS_ADD, destReg, zeroReg, val);
    } else if (src->type == IDENTIFIER) {
        getArg(src, b, destReg);
    } else {
        printf("Invalid type in setRegister %d\n", src->type);
    }
}

void setValue() {}

void mathToInstruction(Block *b, int op, Tac *tac) {
    // regA, regB are used to store the two argument IF necessary. They are
    // used to store values from memory, they are not used for immediate values
    // regC is the out register for the calculation
    // dest is the memory location to store the result in
    // num1, num2 stores either register pointer or immediate value
    Number *regA = newNum(ADDR_REG, REG_T_START, NULL),
           *regB = newNum(ADDR_REG, REG_T_START + 1, NULL),
           *regC = newNum(ADDR_REG, REG_T_START + 2, NULL),
           *dest = getVarLocation(tac->dest, b->frame);
    getArg(tac->src1, b, regA);
    getArg(tac->src2, b, regB);

    if(op == '*'){
        addInstruction(b, op, regA, regB, NULL);
        addInstruction(b, INS_MFLO, regC, NULL, NULL);
        addInstruction(b, INS_SW, regC, dest, NULL);
        return;
    }

    addInstruction(b, op, regC, regA, regB);
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
                case '*':
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
                case RETURN_SCOPE: {
                    Frame *currFrame = block->frame,
                          *prevFrame = block->frame->next;
                    Number *stackPointer = newNum(ADDR_REG, REG_SP, NULL);
                    Number *frameSize;
                    while (currFrame->next != NULL) {
                        frameSize = newNum(
                            ADDR_IMM, currFrame->frameSize * WORD_SIZE, NULL);
                        addInstruction(block, INS_ADD, stackPointer,
                                       stackPointer, frameSize);
                        currFrame = currFrame->next;
                    }
                    break;
                }
                case BRANCH:
                    addInstruction(block, INS_JMP, (Number *)tacList->dest,
                                   NULL, NULL);
                    break;
                case BRANCH_FALSE: {
                    // Branch if the value is 0
                    // Register to load value into
                    Number *reg = newNum(ADDR_REG, REG_T_START, NULL);
                    getArg(tacList->src1, block, reg);
                    addInstruction(block, INS_BZE, (Number *)tacList->dest, reg,
                                   NULL);
                    break;
                }
                case LABEL:
                    addInstruction(block, LABEL, (Number *)tacList->dest, NULL,
                                   NULL);
                    break;
                case DEFINE_FUNC_START: {
                    declare(tacList->dest, block->frame);
                    char buf[256];
                    sprintf(buf, 
                    "la $t0, %s\n"
                    "sw $t0, func_def\n", 
                    tacList->dest->lexeme);
                    strcat(CODE_FUNC_DEF, buf);
                    Frame *funcFrame = allocFrame();
                    funcFrame->next = block->frame;
                    block->frame = funcFrame;
                    break;
                }
                case DEFINE_FUNC_END: {
                    if (block->frame->next == NULL) {
                        perror("No more frames left");
                    }
                    block->frame = block->frame->next;
                    break;
                }
                // RETURN ADDRESS
                case LOAD_RET_ADDR: {
                    Number *returnReg = newNum(ADDR_REG, REG_RA, NULL);
                    getArg(tacList->dest, block, returnReg);
                    break;
                }
                case SAVE_RET_ADDR: {
                    Number *returnReg = newNum(ADDR_REG, REG_RA, NULL);
                    addInstruction(block, INS_SW, returnReg,
                                   getVarLocation(tacList->dest, block->frame),
                                   NULL);
                    break;
                }
                // RETURN ADDRESS
                case LOAD_RET_VAL: {
                    Number *val = newNum(ADDR_REG, REG_RET, NULL),
                           *memLoc =
                               getVarLocation(tacList->dest, block->frame);
                    addInstruction(block, INS_SW, val, memLoc, NULL);
                    break;
                }
                case SAVE_RET_VAL: {
                    Number *val = newNum(ADDR_REG, REG_RET, NULL);
                    getArg(tacList->dest, block, val);
                    break;
                }
                case APPLY:
                    addInstruction(block, INS_JAL, (Number *)tacList->dest,
                                   NULL, NULL);
                    break;
                case RETURN: {
                    addInstruction(block, INS_JPR, NULL, NULL, NULL);
                    break;
                }
                case DEC_ARG: {
                    // Load the value of args into reg t0
                    Number *argsAddress =
                               newNum(ADDR_IDT, 0, (Number *)(ARGS_IDENT)),
                           *argsAddrReg = newNum(ADDR_REG, REG_T_START, NULL);

                    addInstruction(block, INS_LA, argsAddrReg, argsAddress,
                                   NULL);

                    // Put arg value in register
                    Number *varReg = newNum(ADDR_REG, REG_T_START + 1, NULL);
                    getArg(tacList->src1, block, varReg);

                    Number *addrRegPlusOff =
                        newNum(ADDR_BAS, tacList->dest->value, argsAddrReg);

                    addInstruction(block, INS_SW, varReg, addrRegPlusOff, NULL);
                    break;
                }
                case DEF_PARAM: {
                    declare(tacList->src1, block->frame);

                    // Load the value of args into reg t0
                    Number *argsAddress =
                               newNum(ADDR_IDT, 0, (Number *)(ARGS_IDENT)),
                           *argsAddrReg = newNum(ADDR_REG, REG_T_START, NULL);

                    addInstruction(block, INS_LA, argsAddrReg, argsAddress,
                                   NULL);

                    // Move value to register
                    Number *valReg = newNum(ADDR_REG, REG_T_START + 1, NULL);
                    Number *addrRegPlusOff =
                        newNum(ADDR_BAS, tacList->dest->value, argsAddrReg);

                    addInstruction(block, INS_LW, valReg, addrRegPlusOff, NULL);
                    getVarLocation(tacList->src1, block->frame);
                    Number *paramLoc =
                        getVarLocation(tacList->src1, block->frame);

                    addInstruction(block, INS_SW, valReg, paramLoc, NULL);
                    break;
                }
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
    switch (n->addrMode) {
        default:
            printf("invalid address mode\n");
            return;
        case ADDR_REG:
            fprintf(file, "$");
            fprintf(file, "%d", n->value);
            break;
        case ADDR_IMM:
            fprintf(file, "%d", n->value);
            break;
        case ADDR_BAS: {
            Number *base = n->base;
            if (base->addrMode == ADDR_REG && base->value == REG_SP)
                fprintf(file, "%d(", calcVariableOffset(f, n));
            else
                fprintf(file, "%d(", n->value);
            printNum(f, n->base);
            fprintf(file, ")");
            break;
        }
        case ADDR_IDT:
            fprintf(file, "%s", (char *)n->base);
            break;
    }
}

void printLabel(Number *label) {
    TOKEN *l = (TOKEN *)label;

    if (strcmp(l->lexeme, "main") == 0) {
        fprintf(file, "main0");
        return;
    }

    if (l->value != -1) {
        fprintf(file, "%s%d", l->lexeme, l->value);
    } else {
        fprintf(file, "%s", l->lexeme);
    }
}

void printBranch(char *ins, Frame *f, Number *label, Number *val) {
    TOKEN *l = (TOKEN *)label;
    if (val == NULL) {
        fprintf(file, "%s ", ins);
        printLabel(label);
        fprintf(file, "\n");
        return;
    }
    fprintf(file, "%s ", ins);
    printNum(f, val);
    fprintf(file, ", ");
    printLabel(label);
    fprintf(file, "\n");
}

void printMoveStack(int bytesToMove) {
    fprintf(file, "addi $sp, $sp, %d\n", bytesToMove);
}

void printInstruction(char *ins, Frame *f, Number *arg1, Number *arg2,
                      Number *arg3) {
    fprintf(file, "%s ", ins);
    if (arg1 == NULL) {
        fprintf(file, "\n");
        return;
    }
    printNum(f, arg1);
    if (arg2 == NULL) {
        fprintf(file, "\n");
        return;
    }
    fprintf(file, ", ");
    printNum(f, arg2);
    if (arg3 == NULL) {
        fprintf(file, "\n");
        return;
    }
    fprintf(file, ", ");
    printNum(f, arg3);
    fprintf(file, "\n");
}

void printIndent(unsigned int depth) {
    for (int i = 0; i < depth; i++) {
        fprintf(file, "   ");
    }
}

// Pritning goes here
void outputCode(Block *code) {
    unsigned int depth = 0;
    file = fopen("./outputs/out.asm", "w");
    fprintf(file, CODE_DATA);
    fprintf(file, CODE_START);
    fprintf(file, CODE_FUNC_DEF);
    fprintf(file, CODE_CALL_MAIN);

    while (code != NULL) {
        Frame *currFrame = code->frame;
        Inst *i = code->head;
        while (i != NULL) {
            // printIndent(depth);
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
                case INS_LA:
                    printInstruction("la", currFrame, i->arg1, i->arg2, NULL);
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
                case '*':
                    printInstruction("mult", currFrame, i->arg1, i->arg2, NULL);
                    break;
                case INS_MFHI:
                    printInstruction("mfhi", currFrame, i->arg1, NULL, NULL);
                    break;
                case INS_MFLO:
                    printInstruction("mflo", currFrame, i->arg1, NULL, NULL);
                    break;
                case LABEL:
                    printLabel(i->arg1);
                    fprintf(file, ":\n");
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
                case INS_JMP:
                    printBranch("j", currFrame, i->arg1, i->arg2);
                    break;
                case INS_BZE:
                    printBranch("beqz", currFrame, i->arg1, i->arg2);
                    break;
                case INS_JAL:
                    printBranch("jal", currFrame, i->arg1, NULL);
                    break;
                case INS_JPR:
                    printInstruction("jr $ra", currFrame, NULL, NULL, NULL);
                    break;
            }
            i = i->next;
        }
        code = code->next;
    }
    
    fclose(file);
}

// ------------------------------PRINTING-------------------

void toMachineCode(BasicBlock *tree) {
    CODE_FUNC_DEF = (char*)malloc(4096 * sizeof(char));
    CODE_FUNC_DEF[0] = '\0';
    Block *block = allocBlock();
    globalFrame = allocFrame();
    block->frame = globalFrame;

    outputCode(traverseTac(tree, block));
}