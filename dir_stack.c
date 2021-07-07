#include "main.h"
typedef struct dir_stack{
  char            *name;
  struct dir_stack *next;
} linkedList;
linkedList *dirStackTop=NULL;
linkedList *historyList=NULL;
linkedList *pushNode(char *name,linkedList *stackTop)
{
    linkedList *new;
    new=(linkedList*)malloc(sizeof(linkedList));
    if (!new) {
        perror("new_item");
        exit(1);
    }
    if (name) {
        new->name = strdup(name);
        if (!new->name) {
            perror("new_item");
            exit(1);
        }
    }
    new->next=stackTop;
    stackTop=new;
    return stackTop;
}
linkedList *popNode(char *popedNodeName,linkedList *stackTop){
    linkedList *p=stackTop;
    if(stackTop==NULL){
        popedNodeName='\0';
        return NULL;
    }
    popedNodeName=strdup(stackTop->name);
    if (!popedNodeName) {
        perror("popNode error");
        exit(1);
    }

    stackTop=stackTop->next;
    free(p);
    return stackTop;
}
void print_list(linkedList *node){
  if(node==NULL)
    return;
  printf("%s\n",node->name);
  print_list(node->next);
  return;

}
void clear_list(linkedList *node){
  if(node==NULL)
    return;
  clear_list(node->next);
  free(node->name);
  free(node);
}
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

int main(){
    char* a="pushd";
    char* b="dirs";
    char* c="popd";
    /* リストに追加 */
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
  /* ファイルリストの表示 */


  /* ファイルリストの削除 */
  clear_list(dirStackTop);
}