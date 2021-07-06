#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

//builtin functions
void ish_cd(char *args[]);

char *BuiltinCommand[];
void (*ish_builtin_func[]) (char *args[]);
char *ExternalCommand[];

