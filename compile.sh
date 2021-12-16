#!/usr/bin/bash
MC="MachineCode";
IN="Interpreter"
rm ./bin/$MS
rm ./bin/$IN
gcc tac.c machinecode.c ./lexer_parser/*.c -g -o ./bin/$MC -Wall -Wextra
gcc interpreter.c ./lexer_parser/*.c -g -o ./bin/$IN -Wall -Wextra