#include "main.h"
#include "stack.h"
#define HISTORYLEN    32
linkedList *histStackTop=NULL;
int historyLen=0;
void pushHistory(char *args[]);
void hist_list_print(linkedList *node);
void histStackArrange(linkedList *node);

void pushHistory(char *args[]){
    char saveCommandName[256];
    saveCommandName[0]='\0';
    for(int i=0;args[i]!=NULL;i++){
        strcat(saveCommandName,args[i]);
    }
    if(historyLen==HISTORYLEN-1){
            histStackArrange(histStackTop);
    }
    histStackTop=pushNode(saveCommandName,histStackTop);
    return;
}
void hist_list_print(linkedList *node){
  if(node==NULL)
    return;
  print_list(node->next);
  printf("%s\n",node->name);
  return;

}
void histStackArrange(linkedList *node){
    linkedList *p=histStackTop;
    /*2番目に古いコマンドのノードに進む*/
    for(int i=0;i<(HISTORYLEN-2);i++){
        p=p->next;
    }
    /*1番古いコマンドを消して2番目に古いコマンドのnextノードをNULLにする*/
    linkedList *temp=p;
    p->next=NULL;
    free(temp->name);
    free(temp);
}
void history(char *args[]){
    hist_list_print(histStackTop);
    return;
}