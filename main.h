#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "stack.h"
//ish functions
void pushd(char *args[]);
void dirs(char *args[]);
void popd(char *args[]);
void history(char *args[]);
void pushHistory(char *args[]);
//ish builtin funcitons
void ish_cd(char *args[]);


