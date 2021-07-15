#include "main.h"

void ish_cd(char *args[]){
    if(args[1]==NULL){
        if(chdir(getenv("HOME")) != 0){
            perror("cd");
        }
    }else {
         if(chdir(args[1]) != 0){
            perror("cd");
         }
    }
    return ;
}
