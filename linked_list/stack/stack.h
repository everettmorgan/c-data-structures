#include "../linked_list.h"

typedef struct linked_list stack;

int push(stack *stk, int data) {
  return insert(stk, data);
}

int pop(stack *stk) {
  return delete_head(stk);
}

node *peek(stack *stk) {
  return stk->h;
}

void print_stack(stack *stk) {
  print_linked_list(stk);
}
