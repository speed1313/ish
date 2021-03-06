#ifndef STACK_H_
#define STACK_H_
typedef struct dir_stack{
  char            *name;
  struct dir_stack *next;
} linkedList;
typedef struct hist_stack{
  char            *name;
  int             histnum;
  struct hist_stack *next;
} histlinkedList;

linkedList *dirStackTop;
linkedList *aliasStackTop;
histlinkedList *histStackTop;
linkedList *pushNode(char *name,linkedList *stackTop);
linkedList *popNode(char *popedNodeName,linkedList *stackTop);
void print_list(linkedList *node);
void clear_list(linkedList *node);
void hist_clear_list(histlinkedList *node);

#endif