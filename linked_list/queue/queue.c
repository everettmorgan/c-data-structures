#include "../stack/stack.h"

typedef struct queue {
  stack * stk;
} queue;

queue * queue_new(int max_length) {
  queue * q = malloc(sizeof(queue));
  q->stk = stack_new(max_length);
  return q;
}

int queue_enqueue(queue * q, void * d) {
  if (q->stk->ll->length == q->stk->max_length) {
    return 1;
  }
  if (q->stk->ll->length == 0) {
    LL_INSERT(q->stk->ll, d);
  }
  else {
    node * n = linked_list_node_new(d);
    q->stk->ll->tail->next = n;
    q->stk->ll->tail = n;
    q->stk->ll->length += 1;
  }
  return 0;
};

int queue_dequeue(queue * q) {
  if (q->stk->ll->length == 0)
    return 1;
  LL_DEL_HEAD(q->stk->ll);
  return 0;
};

node * queue_peek(queue * q) {
  return q->stk->ll->head;
};

void queue_free(queue * q) {
  stack_free(q->stk);
  free(q);
};

void queue_print(queue * q) {
  linked_list_print(q->stk->ll);
};
