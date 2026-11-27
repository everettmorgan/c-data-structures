/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "../stack/stack.h"

typedef struct queue {
  stack * stk;
  linked_list_allocator allocator;
} queue;

// queue_new_with_allocator : allocates a new queue on the heap using the
// supplied allocator and returns a pointer to it, or NULL when the allocator
// is unusable or any allocation fails.
queue * queue_new_with_allocator(int max_length, linked_list_allocator allocator) {
  if (!stack_allocator_is_valid(allocator)) {
    return NULL;
  }

  queue * q = allocator.allocate(sizeof(*q), allocator.context);
  if (q == NULL) {
    return NULL;
  }

  q->stk = stack_new_with_allocator(max_length, allocator);
  if (q->stk == NULL) {
    allocator.deallocate(q, allocator.context);
    return NULL;
  }

  q->allocator = allocator;
  return q;
}

queue * queue_new(int max_length) {
  return queue_new_with_allocator(max_length, linked_list_default_allocator());
}

// queue_enqueue : appends d to the back of the queue and returns 1 when the
// queue is full, unusable or the node cannot be allocated.
int queue_enqueue(queue * q, void * d) {
  if (q == NULL || !stack_is_usable(q->stk) ||
      q->stk->ll->length == q->stk->max_length) {
    return 1;
  }
  if (q->stk->ll->length == 0) {
    return stack_push(q->stk, d);
  }

  node * n = linked_list_node_new_with_allocator(d, &q->stk->ll->allocator);
  if (n == NULL) {
    return 1;
  }
  q->stk->ll->tail->next = n;
  q->stk->ll->tail = n;
  q->stk->ll->length += 1;
  return 0;
}

// queue_dequeue : removes the front node and returns 1 when the queue is empty.
int queue_dequeue(queue * q) {
  if (q == NULL || !stack_is_usable(q->stk) || q->stk->ll->length == 0) {
    return 1;
  }
  if (linked_list_delete(q->stk->ll, q->stk->ll->head) != LINKED_LIST_OK) {
    return 1;
  }
  return 0;
}

node * queue_peek(queue * q) {
  if (q == NULL || !stack_is_usable(q->stk)) {
    return NULL;
  }
  return q->stk->ll->head;
}

void queue_free(queue * q) {
  if (q == NULL) {
    return;
  }
  if (q->stk != NULL) {
    stack_free(q->stk);
  }
  q->allocator.deallocate(q, q->allocator.context);
}

void queue_print(queue * q) {
  if (q == NULL || !stack_is_usable(q->stk)) {
    return;
  }
  stack_print(q->stk);
}
