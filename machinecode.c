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
        i = (Inst *)malloc(sizeof(Inst));;
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
        return 0;
    }
    unsigned int count = 0;
    Binding *b = f->b;
    while (b != NULL) {
        if (b->var == var) {
            perror("Variable already declared");
        }
        b = b->next;
        count++;
    }

    f->b = allocBinding(var, count);
    return count;
}

int getVarLocation(TOKEN *var, Frame *f) {
    Binding *b = f->b;
    while (b != NULL) {
        if (b->var == var) {
            return b->memLoc;
        }
    }
    if (f->next) {
        perror("Variable not declared");
        return -1;
    }
    getVarLocation(var, f->next);
}

Block *traverseTac(Tac *tree, Frame *f, Block *block) {
  while(tree != NULL){
    switch (tree->op) {
        default:
            perror("Invalid operation");
            break;
        case '~':{
            declare(tree->dest, f);
            break;
        }
        case '=':{
          addInstruction(block, INS_SW, tree->src1->value, getVarLocation(tree->dest, f), 0);
        }
    }
    tree = tree->next;
  }
  return block;
}

// Pritning goes here
void outputCode(Block *code) {
  while(code != NULL) {
    printf("----------\n");
    Inst *i = code->head;
    while(i != NULL){
      switch(i->op) {
        default:
          perror("Invalid instruction");
          break;
        case INS_SW:
          printf("sw %d, %d\n", i->arg1, i->arg2);
      }
      i = i->next;
    }
    code = code->next;
  }
}

void toMachineCode(Tac *tree) {
    Block b;
    Frame f;

    b.head = b.tail = NULL;
    b.next = NULL;
    b.memSize = 0;
    f.b = NULL;
    f.next = NULL;

    outputCode(traverseTac(tree, &f, &b));

}