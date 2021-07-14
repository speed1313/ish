#include "main.h"
void echo(char **argv){
    printf("ish: echo\n");
    for(int i=1;argv[i]!=NULL;i++){
        printf("%s ",argv[i]);
    }
    printf("\n");
    return;
}