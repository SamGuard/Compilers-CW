#ifndef NODES_H
#define NODES_H
#include "token.h"

typedef struct NODE {
  int          type;
  struct NODE *left;
  struct NODE *right;
} NODE;

NODE* make_leaf(TOKEN*);
NODE* make_node(int, NODE*, NODE*);
#endif