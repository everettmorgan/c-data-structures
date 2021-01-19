#include "../linked_list.h"

typedef linked_list_node stack_node;

typedef struct stack {
    linked_list * ll;
    int max_length;
} stack;

// stack_new : allocates a new stack on the heap and
// returns a pointer to the newly allocated stack.
stack * stack_new(int max_length) {
    stack * s = malloc(sizeof(stack));
    s->ll = linked_list_new(0);
    s->max_length = max_length;
    return s;
}

// stack_push : inserts a node at the top of the stack
// and will return 1 if the stack is full.
int stack_push(stack * stk, void * data) {
  if (stk->ll->length == stk->max_length)
    return 1;
  LL_INSERT(stk->ll, data);
  return 0;
}

// stack_pop : pops the topmost node and will return
// 1 if the stack is empty.
int stack_pop(stack * stk) {
  if (stk->ll->length == 0)
    return 1;
  LL_DEL_TAIL(stk->ll);
  return 0;
}

// stack_peek :
stack_node * stack_peek(stack * stk) {
    return stk->ll->tail;
}

void stack_print(stack * stk) {
    linked_list_print(stk->ll);
}

void stack_free(stack * stk) {
    linked_list_free(stk->ll);
    free(stk);
}