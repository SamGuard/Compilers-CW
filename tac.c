#include "tac.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tac *newTac(){
  tac *t = (tac *)malloc(sizeof(tac));
  if(t == NULL){
    perror("cannot allocate memory in newTac\n");
  }
  return t;
}

tac *moveToFront(tac *t){
  while(t->next != NULL){
    t = t->next;
  }
}



void traverse(NODE *tree, tac *prev) {
    switch (tree->type) {
        default:
            perror("unexpected type");
        case 0:
            break;
        case 'D':
            traverse(tree->right, prev);
            return;
        case ';':
            traverse(tree->left, prev);
            traverse(tree->right, prev);
            return;
        case '~':
            traverse(tree->right, prev);
            return;
        case '=':
            traverse(tree->right, prev);
            prev = moveToFront(prev);
            prev->dest = (TOKEN*)tree->left->left;
            return;
        case '+':

            break;
        case '-':
            break;
        case '*':
            break;
        case '/':
            break;
        case '%':
            break;
        case '<':
            break;
        case '>':
            break;
        case EQ_OP:
            break;
        case NE_OP:
            break;
        case LE_OP:
            break;
        case GE_OP:
            break;
        case IF:
            break;
        case WHILE:
            break;
        case LEAF:
            break;
        case APPLY:
            break;
        case BREAK:
            break;
        case RETURN:
            break;
    }
}

void *toTac(NODE* tree) {
  tac head;
  traverse(tree, &head);
}