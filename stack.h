#ifndef STACK_H_
#define STACK_H_
typedef struct dir_stack{
  char            *name;
  struct dir_stack *next;
} linkedList;
linkedList *dirStackTop;
linkedList *histStackTop;

linkedList *pushNode(char *name,linkedList *stackTop);
linkedList *popNode(char *popedNodeName,linkedList *stackTop);
void print_list(linkedList *node);
void clear_list(linkedList *node);

#endif