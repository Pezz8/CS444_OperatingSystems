#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// accept up to 16 command-line arguments
#define MAXARG 16

// allow up to 64 environment variables
#define MAXENV 64

// keep the last 500 commands in history
#define HISTSIZE 500

// accept up to 1024 bytes in one command
#define MAXLINE 1024

static char **parseCmd(char cmdLine[])
{
  char **cmdArg, *ptr;
  int i;

  //(MAXARG + 1) because the list must be terminated by a NULL ptr
  cmdArg = (char **)malloc(sizeof(char *) * (MAXARG + 1));
  if (cmdArg == NULL)
  {
    perror("parseCmd: cmdArg is NULL");
    exit(1);
  }
  for (i = 0; i <= MAXARG; i++) // note the equality
    cmdArg[i] = NULL;
  i = 0;
  ptr = strsep(&cmdLine, " ");
  while (ptr != NULL)
  {
    // (strlen(ptr) + 1)
    cmdArg[i] = (char *)malloc(sizeof(char) * (strlen(ptr) + 1));
    if (cmdArg[i] == NULL)
    {
      perror("parseCmd: cmdArg[i] is NULL");
      exit(1);
    }
    strcpy(cmdArg[i++], ptr);
    if (i == MAXARG)
      break;
    ptr = strsep(&cmdLine, " ");
  }
  return (cmdArg);
}

// Function that return the index of the last enviroment variable + 1
int getNumEnv(char *env[])
{
  int k = 0;
  while (env[k] != NULL)
    k++;
  return k + 1;
}

// Function to print one element of a char array at a set index
void printCharArrElement(char *env[], int index)
{
  printf("%s\n", env[index]);
}

// Function that return if a string starts with another string
bool startsWith(char *string, char *start)
{
  if (string == NULL || start == NULL)
    return false;

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

// Function in case we need to print only a selection of environment variables (I selected only the ones shown in the PDF)
// Was not sure if we needed it but I made it just in case
void printOurEnv(char *env[])
{
  int k = 0;
  while (env[k] != NULL)
  {
    // printf("%s\t\t%d\n", env[k], k);
    if (startsWith(env[k], "SHELL"))
    {
      printCharArrElement(env, k);
    }
    else if (startsWith(env[k], "EDITOR"))
    {
      printCharArrElement(env, k);
    }
    else if (startsWith(env[k], "PWD"))
    {
      printCharArrElement(env, k);
    }
    else if (startsWith(env[k], "PAGER"))
    {
      printCharArrElement(env, k);
    }
    else if (startsWith(env[k], "LOGNAME"))
    {
      printCharArrElement(env, k);
    }
    k++;
  }
}

// Function that prints an entire char array
void printCharArr(char *env[])
{
  int k = 0;
  while (env[k] != NULL)
  {
    printCharArrElement(env, k);
    k++;
  }
}

// Function to set an environment variable from the command received
void setEnv(char *name, char *value, char *env[])
{
  // New environment variable
  char *newEntry;
  // Flag to track if a variable has been added
  bool flag = false;
  int i = 0;
  newEntry = malloc(strlen(name) + strlen(value) + 2);
  sprintf(newEntry, "%s=%s", name, value);

  while (env[i] != NULL)
  {
    if (startsWith(env[i], name))
    {
      // If the existing variable has the same values as the one we want to add
      if (strcmp(env[i], newEntry) == 0)
      { // Error message
        // printf("Enviroment Variable exist already!\n");
        flag = true;
        break;
      }
      // If the existing variable has different values from the ones we want to add
      else
      { // Error message
        // printf("Variable exist different values\n");
        free(env[i]);
        env[i] = malloc(strlen(newEntry));
        env[i] = newEntry;
        flag = true;
        break;
      }
    }
    i++;
  }

  // If a variable still has not been added
  if (flag == false)
  {
    // Check if the current number of variable is less than our MAX
    if (getNumEnv(env) < MAXENV)
    { // DEBUG
      // printf("%d\n", getNumEnv(env));
      env[getNumEnv(env)] = malloc(strlen(newEntry));
      env[getNumEnv(env)] = newEntry;
      // DEBUG
      // printf("%s\n", env[getNumEnv(env)]);
    }
    else
    {
      printf("Environment variable limit reached\n");
    }
  }
}

// Function that removes an environment variable
void unsetEnv(char *name, char *env[])
{
  int i, j;
  // Flag to track if a variable has been removed
  bool flag = false;
  // Loop trough our env array
  for (i = 0; env[i] != NULL && !flag; i++)
  {
    // If we find the enviroment variable that we need to remove
    if (startsWith(env[i], name))
    {
      // Free memeory
      free(env[i]);
      flag = true;
      // Shift the remaining elements of the array to account for the removal.
      for (j = i; env[j + 1] != NULL; j++)
      {
        env[j] = env[j + 1];
      }
      env[j] = NULL;
    }
  }
}

// Function that returns an enviroment variable
char *getEnvirnoment(char *var, char *env[])
{
  int k = 0;
  if (var == NULL)
    return NULL;

  while (env[k] != NULL)
  {
    if (startsWith(env[k], var))
    {
      return env[k] + 1;
    }
    k++;
  }
}

// Function to change directory
void changeDirectory(char *path, char *env[])
{
  // Variable holding our home directory
  char *homeDir;
  char cwd[MAXLINE];

  // If path is empty and does not have any value we will set our path to HOME
  if (path == NULL || strcmp(path, "") == 0)
  {
    homeDir = getEnvirnoment("HOME", env);
    // If there is no variable for HOME
    if (homeDir == NULL)
    {
      // Error message
      printf("Home not set!\n");
      return;
    }
    path = homeDir;
  }

  // Changing directory
  if (chdir(path) != 0)
  {
    perror("chdir");
    return;
  }

  // Getting the current working directory
  if (getcwd(cwd, sizeof(cwd)) == NULL)
  {
    perror("getcwd");
    return;
  }
  // Updating the PWD enviroment variable to the current working directory
  setEnv("PWD", cwd, env);
}

// Function that adds commands to the history array and increments the history size after doing so
void addToHistory(char *command, int *currentHistSize, char *history[])
{
  int index = *currentHistSize % HISTSIZE;

  strncpy(history[index], command, MAXLINE - 1);
  history[index][MAXLINE - 1] = '\0';
  (*currentHistSize)++;
}

// Function that prints all the element of our history size starting from the oldest to the newest
void printHistory(int currentSize, char *history[])
{
  // If the currentSize of history is less than our 500 limit we start at index 0 else we use the currentSize % HISTSIZE as our starting position
  int start = currentSize < HISTSIZE ? 0 : currentSize % HISTSIZE;
  // If the currentSize of history is less than our 500 limit we end at currentSize else we use HISTSIZE as our end position
  int end = currentSize < HISTSIZE ? currentSize : HISTSIZE;

  // We for the elements present in the array
  for (int i = start; i < end; i++)
  {
    // We print the elements at index i%HISTSIZE (ex. i == 502 and HISTSIZE == 500 we will be printing index 2)
    printf("%d: %s\n", i + 1, history[i % HISTSIZE]);
  }
}

// Function that loads the enviroment variables stored in envp to our local array.
void loadEnv(char *source[], char *target[])
{
  int i;
  for (i = 0; source[i] != NULL && i < MAXENV; i++)
  {
    target[i] = malloc(strlen(source[i]) + 1);
    if (target[i] == NULL)
    {
      printf("Memory allocation failed");
      exit(1);
    }
    // DEBUG
    // if (i == 30)
    //   break;
    if (i == MAXENV - 1)
    {
      target[i] = NULL;
      break;
    }
    strcpy(target[i], source[i]);
  }
  if (i < MAXENV)
    target[i] = NULL;
}

int main(int argc, char *argv[], char *envp[])
{
  char cmdLine[MAXLINE], **cmdArg;
  int status, i, debug;
  pid_t pid;

  // Counter for the current history size.
  int currentHistSize = 0;
  // String to copy the line to pass into history from cmdLine
  char *cmdLineCpy;

  char *history[HISTSIZE];

  // Allocating memory for history
  for (int i = 0; i < HISTSIZE; i++)
  {
    history[i] = malloc(MAXLINE);
    if (history[i] == NULL)
    {
      printf("Failed to allocate memory\n");
      for (int j = 0; j < i; j++)
      {
        free(history[j]);
      }
      return 1;
    }
  }

  // Array holding our local environment variables
  char *envVar[MAXENV];
  // Loading the variables to the array
  loadEnv(envp, envVar);

  debug = 0;
  i = 1;
  while (i < argc)
  {
    if (!strcmp(argv[i], "-d"))
      debug = 1;
    i++;
  }
  while ((1))
  {
    printf("bsh> ");                     // prompt
    fgets(cmdLine, MAXLINE, stdin);      // get a line from keyboard
    cmdLine[strlen(cmdLine) - 1] = '\0'; // strip '\n'
    // Allocating memory
    cmdLineCpy = malloc(strlen(cmdLine) + 1);
    // Coping the cmdLine to pass to history
    strcpy(cmdLineCpy, cmdLine);
    addToHistory(cmdLine, &currentHistSize, history);

    cmdArg = parseCmd(cmdLine);
    if (debug)
    {
      i = 0;
      while (cmdArg[i] != NULL)
      {
        printf("\t%d (%s)\n", i, cmdArg[i]);
        i++;
      }
    }

    // built-in command exit
    if (strcmp(cmdArg[0], "exit") == 0)
    {
      if (debug)
        printf("exiting\n");
      break;
    }
    // built-in command env
    else if (strcmp(cmdArg[0], "env") == 0)
    {
      // printOurEnv(envVar);
      printCharArr(envVar);
    }
    // built-in command setenv
    else if (strcmp(cmdArg[0], "setenv") == 0)
    {
      setEnv(cmdArg[1], cmdArg[2], envVar);
      // DEBUG
      // setEnv("PAGER", "more", envVar);
      // setEnv("EDITOR", "VSCODE", envVar);
    }
    // built-in command unsetenv
    else if (strcmp(cmdArg[0], "unsetenv") == 0)
    {
      unsetEnv(cmdArg[1], envVar);
      // DEBUG
      // unsetEnv("PAGER", envVar);
    }
    // built-in command cd
    else if (strcmp(cmdArg[0], "cd") == 0)
    {
      changeDirectory(cmdArg[1], envVar);
    }
    // built-in command history
    else if (strcmp(cmdArg[0], "history") == 0)
    {
      printHistory(currentHistSize, history);
    }

    // implement how to execute Minix commands here

    // the following is a template for using fork() and execv()
    //***remove this else case from your final code for bsh.c
    // else
    // {
    //   if (debug)
    //     printf("calling fork()\n");
    //   pid = fork();
    //   if (pid != 0)
    //   {
    //     if (debug)
    //       printf("parent %d waiting for child %d\n", getpid(), pid);
    //     waitpid(pid, &status, 0);
    //   }
    //   else
    //   {
    //     status = execv(cmdArg[0], cmdArg);
    //     if (status)
    //     {
    //       printf("\tno such command (%s)\n", cmdArg[0]);
    //       return 1;
    //     }
    //   }
    // }
    // //***remove up to this point

    // clean up before running the next command
    i = 0;
    while (cmdArg[i] != NULL)
      free(cmdArg[i++]);
    free(cmdArg);
  }

  return 0;
}