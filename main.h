#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
//ish functions
void ish_cd(char *args[]);
void pushd(char *args[]);
void dirs(char *args[]);
void popd(char *args[]);
void history(char *args[]);
//ish builtin funcitons
char *BuiltinCommand[];
void (*builtin_func[]) (char *args[]);
void (*external_func[]) (char *args[]);
char *ExternalCommand[];

