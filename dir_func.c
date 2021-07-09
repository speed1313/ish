#include "main.h"
#include "stack.h"
linkedList *dirStackTop=NULL;
void pushd(char *args[]){
    char cwd[256];
     if (getcwd(cwd, sizeof(cwd)) == NULL){
        perror("getcwd() error");
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
    dirStackTop=popNode(popdDirName,dirStackTop);
    return;
}

/*
int main(){
    char* a="pushd";
    char* b="dirs";
    char* c="popd";
        for(;;){
        char **args;
        char func[20];
        scanf("%s",func);

        if(strcmp(func,a)==0){
            printf("call pushd\n");
            pushd(args);
        }
        else if(strcmp(func,b)==0){
            dirs(args);
        }else if(strcmp(func,c)==0){
            popd(args);
        }
    }
  clear_list(dirStackTop);
}
*/
