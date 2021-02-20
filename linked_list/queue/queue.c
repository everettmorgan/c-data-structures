/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

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
      stack_push(q->stk, d);
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
  stack_pop(q->stk);
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
  stack_print(q->stk);
};
