#include "main.h"

void prompt(char *args[]){
    int argc=0;
    for(int i=0;args[i]!=NULL;i++){
        argc++;
    }
    if(argc!=2){
        strcpy(prompt_str,default_prompt);
    }else{
        strcpy(prompt_str,args[1]);
    }
    return;

}