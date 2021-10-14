#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

char lexerParserPath[] = "./Lexer_parser.exe";

int main()
{

  int *lexerParserPID = (int *)malloc(2 * sizeof(int));
  lexerParserPID[0] = -1;
  lexerParserPID[1] = -1;

  int pid = fork();
  printf("%d\n", pid);
  if (pid != 0)
  {
    lexerParserPID[1] = pid;
    printf("%d\n", lexerParserPID[1]);
    char *args[] = {lexerParserPath, NULL};
    execvp(args[0], args);
    return 0;
  }
  else
  {
    lexerParserPID[0] = pid;
  }

  while (1 == 1)
  {
    printf("Lexer PID: %d\n", lexerParserPID[1]);
    if (lexerParserPID[1] != -1)
    {
      break;
    }
  }
  printf("Lexer PID: %d\n", lexerParserPID[1]);
  printf("Ending-----");

  return 0;
}