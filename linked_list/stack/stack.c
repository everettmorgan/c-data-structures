/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "../linked_list.h"

typedef struct stack {
    linked_list * ll;
    int max_length;
    linked_list_allocator allocator;
} stack;

/* An allocator must be able to hand out and release memory. */
static int stack_allocator_is_valid(linked_list_allocator allocator) {
  return allocator.allocate != NULL && allocator.deallocate != NULL;
}

/* A stack is only usable once it and its backing list exist. */
static int stack_is_usable(const stack * stk) {
  return stk != NULL && stk->ll != NULL;
}

// stack_new_with_allocator : allocates a new stack on the heap using the
// supplied allocator and returns a pointer to it, or NULL when the allocator
// is unusable or any allocation fails.
stack * stack_new_with_allocator(int max_length, linked_list_allocator allocator) {
  if (!stack_allocator_is_valid(allocator)) {
    return NULL;
  }

  stack * s = allocator.allocate(sizeof(*s), allocator.context);
  if (s == NULL) {
    return NULL;
  }

  s->ll = linked_list_new_with_allocator(LINKED_LIST_SINGLY, allocator);
  if (s->ll == NULL) {
    allocator.deallocate(s, allocator.context);
    return NULL;
  }

  s->max_length = max_length;
  s->allocator = allocator;
  return s;
}

// stack_new : allocates a new stack on the heap and
// returns a pointer to the newly allocated stack.
stack * stack_new(int max_length) {
  return stack_new_with_allocator(max_length, linked_list_default_allocator());
}

// stack_push : inserts a node at the top of the stack and will return 1 if the
// stack is full, unusable or the node cannot be allocated.
int stack_push(stack * stk, void * data) {
  if (!stack_is_usable(stk) || stk->ll->length == stk->max_length) {
    return 1;
  }
  if (ll_insert(stk->ll, data) != LINKED_LIST_OK) {
    return 1;
  }
  return 0;
}

// stack_pop : pops the topmost node and will return
// 1 if the stack is empty.
int stack_pop(stack * stk) {
  if (!stack_is_usable(stk) || stk->ll->length == 0) {
    return 1;
  }
  if (ll_drop(stk->ll) != LINKED_LIST_OK) {
    return 1;
  }
  return 0;
}

// stack_peek : returns the topmost node without removing it.
node * stack_peek(stack * stk) {
  if (!stack_is_usable(stk)) {
    return NULL;
  }
  return stk->ll->tail;
}

void stack_print(stack * stk) {
  if (!stack_is_usable(stk)) {
    return;
  }
  ll_print(stk->ll);
}

void stack_free(stack * stk) {
  if (stk == NULL) {
    return;
  }
  if (stk->ll != NULL) {
    ll_free(stk->ll);
  }
  stk->allocator.deallocate(stk, stk->allocator.context);
}
