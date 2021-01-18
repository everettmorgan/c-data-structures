#include "../stack/stack.h"

typedef linked_list_node queue_node;

typedef struct queue {
  stack * stk;
} queue;

queue * queue_new(int max_length) {
  queue * q = malloc(sizeof(queue));
  q->stk = stack_new(max_length);
}

