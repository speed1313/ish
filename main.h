#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "stack.h"
#include <ctype.h>
#include <dirent.h>

//global varibles
char default_prompt[];
char *prompt_str;
//ish functions


//ish builtin funcitons
void ish_cd(char *args[]);
void pushd(char *args[]);
void dirs(char *args[]);
void popd(char *args[]);
void history(char *args[]);
void pushHistory(char *args[]);
void prompt(char *args[]);



