#include "stack.h"
#include "main.h"
linkedList *aliasStackTop=NULL;
void alias(char *args[]);
void unalias(char *args[]);
linkedList *delete_alias(char *alias,linkedList *parent);
void pushalias(char *args[]);
char *search_alias(char *arg,linkedList *node);

void alias(char *args[]){
    int argc=0;
    for(int i=0;args[i]!=NULL;i++){
        argc++;
    }
    if(argc==1){
        print_list(aliasStackTop);
    }else if(argc==3){
        pushalias(args);
    }else{
        fprintf(stderr,"usage: alias [alias_name] [command]\n");
    }
    return;
}
void unalias(char *args[]){
    int argc=0;
    for(int i=0;args[i]!=NULL;i++){
        argc++;
    }
    if(argc==2){
        aliasStackTop=delete_alias(alias_tmp,aliasStackTop);
    }else{
        fprintf(stderr,"usage: unalias [alias_name]\n");
    }
    return;

}
linkedList *delete_alias(char *alias,linkedList *node){
    if(node==NULL){
        fprintf(stderr,"unalias: no such hash table element: %s\n",alias);
        return node;
    }
    char buf[256];
    int i;
    for(i=0;(node->name[i]!='\0')&&(!isspace(node->name[i]));i++){
        buf[i]=node->name[i];
    }
    buf[i]='\0';
    if(strcmp(buf,alias)==0){
        linkedList *tmp;
        tmp=node;
        node=node->next;
        free(tmp->name);
        free(tmp);
        return node;
    }else{
        return node=delete_alias(alias,node->next);
    }
}
void pushalias(char *args[]){
    char Alias[256];
    sprintf(Alias,"%s %s",args[1],args[2]);
    aliasStackTop=pushNode(Alias,aliasStackTop);
    return;
}
char *search_alias(char *arg,linkedList *node){
    if(node==NULL){
        return arg;
    }
    char buf[256];
    int i=0;
    for(i=0;(node->name[i]!='\0')&&(!isspace(node->name[i]));i++){
        buf[i]=node->name[i];
    }
    buf[i]='\0';
    if(strcmp(arg,buf)==0){
        return &(node->name[i+1]);
    }else{
        return search_alias(arg,node->next);
    }
}