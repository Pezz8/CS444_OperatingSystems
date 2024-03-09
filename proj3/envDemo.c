#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

bool startsWith(char *string, char *start)
{
  int string_length = strlen(string);
  int start_length = strlen(start);

  if (start_length > string_length)
    return false;

  for (int i = 0; i < start_length; i++)
  {
    if (string[i] != start[i])
      return false;
  }

  return true;
}

// env var are sent in as the third parameter
int main(int argc, char *argv[], char *envp[])
{
  int k;

  k = 0;
  while (envp[k] != NULL)
  {
    printf("%s\n", envp[k]);
    k++;
    /*
    if (startsWith(envp[k], "SHELL")) // 49
    {
      printf("%d\n", k);
    }
    else if (startsWith(envp[k], "EDITOR")) //
    {
      printf("%d\n", k);
    }
    else if (startsWith(envp[k], "PWD")) // 57
    {
      printf("%d\n", k);
    }
    else if (startsWith(envp[k], "LOGNAME")) // 22
    {
      printf("%d\n", k);
    }
    */
  }
  printf("\n%d env variables\n", k);
  return 0;
}
