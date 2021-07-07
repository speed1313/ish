typedef struct dir_stack{
  char            *name;
  struct dir_stack *next;
} linkedList;
linkedList *pushNode(char *name,linkedList *stackTop);
linkedList *popNode(char *popedNodeName,linkedList *stackTop);
void print_list(linkedList *node);
void clear_list(linkedList *node);