#ifndef __INTERPRETER_H
#define __INTERPRETER_H
#include "./lexer_parser/nodes.h"
#include "./lexer_parser/C.tab.h"
#include "./lexer_parser/token.h"

void interpreter(NODE *tree);

#endif