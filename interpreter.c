#include "interpreter.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct Binding
{
  int value;
  TOKEN *name;
  struct Binding *next;
} Binding;

typedef struct Frame
{
  Binding *b;
  struct Frame *next;
} Frame;

TOKEN *evalExpression(NODE* exp, Frame *f){
  TOKEN *t = (TOKEN*)malloc(sizeof(TOKEN));
  
}

void declare(TOKEN *x, Frame *f){

}

int traverse(NODE *tree, Frame *f)
{

  switch (tree->type)
  {
  default:
    error("unexpected type");
    return 0;
  case 'D':
    return traverse(tree->right, f);
  case ';':
    traverse(tree->left, f);
    if(tree->right != NULL) traverse(tree->right, f);
    return;
  case '~':
    traverse(tree->right, f);
    return;
  case '=':
    declare(evalExpression(tree, f), f);
    return 0;
    
  }
}

void interpreter(NODE *tree)
{
  Frame *f = (Frame *)malloc(sizeof(Frame));
  f->b = (Binding *)malloc(sizeof(Binding));
  f->next = NULL;
}

void simplePrintTree(NODE *tree)
{
  NODE *head = tree;
  printf("type: %c  ", head->type);
  if (head->right == NULL)
  {
    printf("Leaf value: %d\n", ((TOKEN *)head->left)->value);
  }
  printf("\n");
  interpreter(head->left);
  interpreter(head->right);
}