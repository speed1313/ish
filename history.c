#include "main.h"
#include "stack.h"
#define HISTORYLEN    32

histlinkedList *histStackTop=NULL;
int histStackLen=0;
int histLen=0;
void pushHistory(char *args[]);
void hist_list_print(histlinkedList *node);
void histStackArrange(histlinkedList *node);
void hist_clear_list(histlinkedList *node);
histlinkedList *hist_pushNode(char *name,histlinkedList *stackTop,int histLen);
histlinkedList *hist_popNode(char *popedNodeName,histlinkedList *stackTop);

void pushHistory(char *args[]){
    char saveCommandName[256];
    saveCommandName[0]='\0';
    for(int i=0;args[i]!=NULL;i++){//parseして分割された引数を連結
        strcat(saveCommandName,args[i]);
        strcat(saveCommandName," ");
    }
    if(histStackLen==HISTORYLEN){//スタックが32であれば一個消す
            histStackArrange(histStackTop);
            histStackLen--;
    }
    histStackTop=hist_pushNode(saveCommandName,histStackTop, histLen);
    histStackLen++;
    histLen++;
    return;
}
void hist_list_print(histlinkedList *node){
  if(node==NULL){
    return;
  }
  hist_list_print(node->next);
  printf("%2d %s\n",node->histnum,node->name);
  return;

}
void histStackArrange(histlinkedList *node){
    histlinkedList *p=histStackTop;
    /*2番目に古いコマンドのノードに進む*/
    while((p->next->next)!=NULL){
        p=p->next;
    }
    /*1番古いコマンドを消して2番目に古いコマンドのnextノードをNULLにする*/
    histlinkedList *temp=p->next;
    p->next=NULL;
    free(temp->name);
    free(temp);
}
void history(char *args[]){
    hist_list_print(histStackTop->next);
    return;
}


histlinkedList *hist_pushNode(char *name,histlinkedList *stackTop,int histLen)
{
    histlinkedList *new;
    new=(histlinkedList*)malloc(sizeof(histlinkedList));
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
    new->histnum=histLen;
    new->next=stackTop;
    stackTop=new;
    return stackTop;
}
histlinkedList *hist_popNode(char *popedNodeName,histlinkedList *stackTop){
    histlinkedList *p=stackTop;
    if(stackTop==NULL){
        popedNodeName='\0';
        return NULL;
    }
    popedNodeName=strdup(stackTop->name);
    if (!popedNodeName) {
        perror("hist_popNode");
        exit(1);
    }
    stackTop=stackTop->next;
    free(p->name);
    free(p);
    return stackTop;
}

void hist_clear_list(histlinkedList *node){
  if(node==NULL)
    return;
  hist_clear_list(node->next);
  free(node->name);
  free(node);
}
