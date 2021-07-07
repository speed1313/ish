#include "main.h"
#include "stack.h"
#define HISTORYLEN    32
linkedList *histStackTop=NULL;
int historyLen=0;
void pushHistory(char *args[]);
void hist_list_print(linkedList *node,int i);
void histStackArrange(linkedList *node);

void pushHistory(char *args[]){
    char saveCommandName[256];
    saveCommandName[0]='\0';
    for(int i=0;args[i]!=NULL;i++){
        strcat(saveCommandName,args[i]);
    }
    if(historyLen==HISTORYLEN){
            histStackArrange(histStackTop);
            historyLen--;
    }
    histStackTop=pushNode(saveCommandName,histStackTop);
    historyLen++;
    return;
}
void hist_list_print(linkedList *node,int i){
  if(node==NULL)
    return;
  int j=i;
  i--;
  hist_list_print(node->next,i);
  printf("%2d %s\n",j,node->name);
  return;

}
void histStackArrange(linkedList *node){
    linkedList *p=histStackTop;
    /*2番目に古いコマンドのノードに進む*/
    while((p->next->next)!=NULL){
        p=p->next;
    }
    /*1番古いコマンドを消して2番目に古いコマンドのnextノードをNULLにする*/
    linkedList *temp=p->next;
    p->next=NULL;
    free(temp->name);
    free(temp);
}
void history(char *args[]){
    hist_list_print(histStackTop,historyLen);
    return;
}