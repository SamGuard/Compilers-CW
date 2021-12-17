#!/usr/bin/bash
AS="toAssembly";
IN="Interpreter"
rm ./bin/$AS
rm ./bin/$IN
gcc tac.c toAssembly.c ./lexer_parser/*.c -g -o ./bin/$AS -Wall -Wextra
gcc interpreter.c ./lexer_parser/*.c -g -o ./bin/$IN -Wall -Wextra