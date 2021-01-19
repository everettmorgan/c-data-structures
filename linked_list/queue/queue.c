#include "../stack/stack.h"

typedef linked_list_node queue_node;

typedef struct queue {
  stack * stk;
} queue;

queue * queue_new(int max_length) {
  queue * q = malloc(sizeof(queue));
  q->stk = stack_new(max_length);
  return q;
}

int queue_enqueue(queue * q, void * d) {
  if (q->stk->ll->length == q->stk->max_length)
    return 1;
  if (q->stk->ll->length == 0)
    stack_push(q->stk, d);
  else {
    stack_node * n = linked_list_node_new(d);
    q->stk->ll->tail->next = n;
    q->stk->ll->tail = n;
    q->stk->ll->length += 1;
  }
  return 0;
};

int queue_dequeue(queue * q) {
  if (q->stk->ll->length == 0)
    return 1;
  linked_list_delete(q->stk->ll, q->stk->ll->head);
  return 0;
};

queue_node * queue_peek(queue * q) {
  return q->stk->ll->head;
};

void queue_free(queue * q) {
  stack_free(q->stk);
  free(q);
};

void queue_print(queue * q) {
  linked_list_print(q->stk->ll);
};
