#include "main.h"
typedef struct dir_stack{
  char            *name;
  struct dir_stack *next;
} linkedList;
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