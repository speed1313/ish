#include "main.h"
#include "stack.h"
linkedList *dirStackTop=NULL;
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
void pushd(char *args[]){
    char cwd[512];
     if (getcwd(cwd, sizeof(cwd)) == NULL){
        perror("getcwd()");
     }else{
         dirStackTop=pushNode(cwd,dirStackTop);
     }
     return;
}
void dirs(char *args[]){
    print_list(dirStackTop);
    return;
}
void popd(char *args[]){
    char popdDirName[256];
    if(dirStackTop==NULL){
        fprintf(stderr,"popd: directory stack empty\n");
    }else {
         if(chdir(dirStackTop->name) != 0){
            perror("popd");
         }
    }
    dirStackTop=popNode(popdDirName,dirStackTop);

    return;
}
