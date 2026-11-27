/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

/* Inject failures into the default allocator only while compiling the library. */
static int reject_default_allocations;

static void *test_default_malloc(size_t size) {
  if (reject_default_allocations) {
    return NULL;
  }
  return malloc(size);
}

#define malloc test_default_malloc
#include "stack.h"
#undef malloc

enum {
  NO_ALLOCATION_FAILURE = 0,
  STACK_ALLOCATION_ATTEMPT = 1,
  LIST_ALLOCATION_ATTEMPT = 2,
  FIRST_NODE_ALLOCATION_ATTEMPT = 3,
  SECOND_NODE_ALLOCATION_ATTEMPT = 4
};

typedef struct allocation_tracker {
  size_t attempts;
  size_t allocations;
  size_t deallocations;
  size_t outstanding;
  size_t fail_on_attempt;
  size_t last_requested_size;
} allocation_tracker;

static void *tracked_allocate(size_t size, void *context) {
  allocation_tracker *tracker = context;
  assert(tracker != NULL);
  tracker->attempts++;
  tracker->last_requested_size = size;
  if (tracker->attempts == tracker->fail_on_attempt) {
    return NULL;
  }

  void *allocation = malloc(size);
  if (allocation != NULL) {
    tracker->allocations++;
    tracker->outstanding++;
  }
  return allocation;
}

static void tracked_deallocate(void *allocation, void *context) {
  if (allocation == NULL) {
    return;
  }

  allocation_tracker *tracker = context;
  assert(tracker != NULL);
  assert(tracker->outstanding > 0);
  tracker->deallocations++;
  tracker->outstanding--;
  free(allocation);
}

static linked_list_allocator tracking_allocator(allocation_tracker *tracker) {
  linked_list_allocator allocator = {
    .allocate = tracked_allocate,
    .deallocate = tracked_deallocate,
    .context = tracker
  };
  return allocator;
}

static void assert_allocation_counts(const allocation_tracker *tracker,
                                     size_t attempts, size_t allocations,
                                     size_t deallocations) {
  assert(deallocations <= allocations);
  assert(tracker->attempts == attempts);
  assert(tracker->allocations == allocations);
  assert(tracker->deallocations == deallocations);
  assert(tracker->outstanding == allocations - deallocations);
}

/* Values are listed from the bottom of the stack to the top. */
static void assert_stack_nodes(const stack *stk, void *const *values,
                               size_t count) {
  assert(stk != NULL);
  assert(stk->ll != NULL);
  assert(stk->ll->type == LINKED_LIST_SINGLY);
  assert(stk->ll->length >= 0);
  assert((size_t)stk->ll->length == count);

  if (count == 0) {
    assert(stk->ll->head == NULL);
    assert(stk->ll->tail == NULL);
    return;
  }

  assert(stk->ll->head != NULL);
  assert(stk->ll->tail != NULL);
  node *current = stk->ll->head;
  for (size_t i = 0; i < count; i++) {
    assert(current != NULL);
    assert(current->data == values[i]);
    assert(current->prev == NULL);
    assert((current == stk->ll->tail) == (i + 1 == count));
    current = current->next;
  }
  assert(current == NULL);
}

static void assert_stack_unchanged(const stack *stk, node *head, node *tail,
                                   void *const *values, size_t count) {
  assert(stk->ll->head == head);
  assert(stk->ll->tail == tail);
  assert_stack_nodes(stk, values, count);
}

static void test_new_stack_state(void) {
  allocation_tracker tracker = { 0 };
  stack *stk = stack_new_with_allocator(3, tracking_allocator(&tracker));

  assert(stk != NULL);
  assert(stk->max_length == 3);
  assert(stk->allocator.allocate == tracked_allocate);
  assert(stk->allocator.deallocate == tracked_deallocate);
  assert(stk->allocator.context == &tracker);
  assert(stk->ll->allocator.allocate == tracked_allocate);
  assert(stk->ll->allocator.context == &tracker);
  assert_stack_nodes(stk, NULL, 0);
  assert(tracker.last_requested_size == sizeof(linked_list));
  assert_allocation_counts(&tracker, 2, 2, 0);

  stack_free(stk);
  assert_allocation_counts(&tracker, 2, 2, 2);
}

static void test_push_pop_peek(void) {
  int first = 1;
  int second = 2;
  int third = 3;
  void *single[] = { &first };
  void *both[] = { &first, &second };
  void *all[] = { &first, &second, &third };
  stack *stk = stack_new(3);

  assert(stk != NULL);
  assert(stack_peek(stk) == NULL);
  assert(stack_pop(stk) == 1);

  assert(stack_push(stk, &first) == 0);
  assert_stack_nodes(stk, single, 1);
  assert(stack_peek(stk) == stk->ll->tail);
  assert(stack_peek(stk)->data == &first);

  assert(stack_push(stk, &second) == 0);
  assert_stack_nodes(stk, both, 2);
  assert(stack_peek(stk)->data == &second);

  assert(stack_push(stk, &third) == 0);
  assert_stack_nodes(stk, all, 3);
  assert(stack_peek(stk)->data == &third);

  assert(stack_pop(stk) == 0);
  assert_stack_nodes(stk, both, 2);
  assert(stack_peek(stk)->data == &second);

  assert(stack_pop(stk) == 0);
  assert_stack_nodes(stk, single, 1);

  assert(stack_pop(stk) == 0);
  assert_stack_nodes(stk, NULL, 0);
  assert(stack_peek(stk) == NULL);
  assert(stack_pop(stk) == 1);

  stack_print(stk);
  stack_free(stk);
}

static void test_capacity_limit(void) {
  int first = 1;
  int second = 2;
  int rejected = 3;
  void *full[] = { &first, &second };
  allocation_tracker tracker = { 0 };
  stack *stk = stack_new_with_allocator(2, tracking_allocator(&tracker));
  assert(stk != NULL);

  assert(stack_push(stk, &first) == 0);
  assert(stack_push(stk, &second) == 0);
  assert_allocation_counts(&tracker, 4, 4, 0);
  node *head = stk->ll->head;
  node *tail = stk->ll->tail;

  assert(stack_push(stk, &rejected) == 1);
  assert_stack_unchanged(stk, head, tail, full, 2);
  assert_allocation_counts(&tracker, 4, 4, 0);

  stack_free(stk);
  assert_allocation_counts(&tracker, 4, 4, 4);
}

static void test_null_stack_guards(void) {
  int value = 1;

  stack_free(NULL);
  stack_print(NULL);
  assert(stack_push(NULL, &value) == 1);
  assert(stack_push(NULL, NULL) == 1);
  assert(stack_pop(NULL) == 1);
  assert(stack_peek(NULL) == NULL);
}

static void test_stack_without_backing_list(void) {
  int value = 1;
  allocation_tracker tracker = { 0 };
  stack detached = {
    .ll = NULL,
    .max_length = 4,
    .allocator = tracking_allocator(&tracker)
  };

  assert(stack_push(&detached, &value) == 1);
  assert(stack_pop(&detached) == 1);
  assert(stack_peek(&detached) == NULL);
  stack_print(&detached);
  assert_allocation_counts(&tracker, 0, 0, 0);
}

static void test_invalid_allocators(void) {
  allocation_tracker tracker = { 0 };
  const linked_list_allocator invalid_allocators[] = {
    { .allocate = NULL, .deallocate = tracked_deallocate, .context = &tracker },
    { .allocate = tracked_allocate, .deallocate = NULL, .context = &tracker },
    { .allocate = NULL, .deallocate = NULL, .context = &tracker }
  };

  for (size_t i = 0;
       i < sizeof(invalid_allocators) / sizeof(invalid_allocators[0]); i++) {
    assert(stack_new_with_allocator(4, invalid_allocators[i]) == NULL);
  }
  assert_allocation_counts(&tracker, 0, 0, 0);
}

static void test_stack_allocation_failure(void) {
  allocation_tracker tracker = { .fail_on_attempt = STACK_ALLOCATION_ATTEMPT };

  assert(stack_new_with_allocator(4, tracking_allocator(&tracker)) == NULL);
  assert(tracker.last_requested_size == sizeof(stack));
  assert_allocation_counts(&tracker, 1, 0, 0);
}

static void test_list_allocation_failure(void) {
  allocation_tracker tracker = { .fail_on_attempt = LIST_ALLOCATION_ATTEMPT };

  assert(stack_new_with_allocator(4, tracking_allocator(&tracker)) == NULL);
  assert(tracker.last_requested_size == sizeof(linked_list));
  assert_allocation_counts(&tracker, 2, 1, 1);
  assert(tracker.outstanding == 0);
}

static void test_first_node_allocation_failure(void) {
  int value = 11;
  void *values[] = { &value };
  allocation_tracker tracker = { .fail_on_attempt = FIRST_NODE_ALLOCATION_ATTEMPT };
  stack *stk = stack_new_with_allocator(4, tracking_allocator(&tracker));
  assert(stk != NULL);
  assert_allocation_counts(&tracker, 2, 2, 0);

  assert(stack_push(stk, &value) == 1);
  assert(tracker.last_requested_size == sizeof(node));
  assert_stack_nodes(stk, NULL, 0);
  assert_allocation_counts(&tracker, 3, 2, 0);

  tracker.fail_on_attempt = NO_ALLOCATION_FAILURE;
  assert(stack_push(stk, &value) == 0);
  assert_stack_nodes(stk, values, 1);
  assert_allocation_counts(&tracker, 4, 3, 0);

  stack_free(stk);
  assert_allocation_counts(&tracker, 4, 3, 3);
  assert(tracker.outstanding == 0);
}

static void test_second_node_allocation_failure(void) {
  int first = 11;
  int second = 22;
  void *single[] = { &first };
  void *both[] = { &first, &second };
  allocation_tracker tracker = { .fail_on_attempt = SECOND_NODE_ALLOCATION_ATTEMPT };
  stack *stk = stack_new_with_allocator(4, tracking_allocator(&tracker));
  assert(stk != NULL);
  assert(stack_push(stk, &first) == 0);
  assert_stack_nodes(stk, single, 1);
  node *head = stk->ll->head;
  node *tail = stk->ll->tail;

  assert(stack_push(stk, &second) == 1);
  assert(tracker.last_requested_size == sizeof(node));
  assert_stack_unchanged(stk, head, tail, single, 1);
  assert_allocation_counts(&tracker, 4, 3, 0);

  tracker.fail_on_attempt = NO_ALLOCATION_FAILURE;
  assert(stack_push(stk, &second) == 0);
  assert_stack_nodes(stk, both, 2);
  assert_allocation_counts(&tracker, 5, 4, 0);

  stack_free(stk);
  assert_allocation_counts(&tracker, 5, 4, 4);
  assert(tracker.outstanding == 0);
}

static void test_unusable_list_allocator(void) {
  int value = 11;
  allocation_tracker tracker = { 0 };
  stack *stk = stack_new_with_allocator(4, tracking_allocator(&tracker));
  assert(stk != NULL);
  linked_list_allocator valid_allocator = stk->ll->allocator;

  stk->ll->allocator.allocate = NULL;
  assert(stack_push(stk, &value) == 1);
  assert_stack_nodes(stk, NULL, 0);

  stk->ll->allocator.deallocate = NULL;
  assert(stack_push(stk, &value) == 1);
  assert_stack_nodes(stk, NULL, 0);

  stk->ll->allocator = valid_allocator;
  assert(stack_push(stk, &value) == 0);
  assert_allocation_counts(&tracker, 3, 3, 0);

  stack_free(stk);
  assert_allocation_counts(&tracker, 3, 3, 3);
}

static void test_default_allocator_failures(void) {
  int value = 11;
  void *values[] = { &value };

  reject_default_allocations = 1;
  assert(stack_new(4) == NULL);
  reject_default_allocations = 0;

  stack *stk = stack_new(4);
  assert(stk != NULL);
  reject_default_allocations = 1;
  assert(stack_push(stk, &value) == 1);
  reject_default_allocations = 0;
  assert_stack_nodes(stk, NULL, 0);
  assert(stack_push(stk, &value) == 0);
  assert_stack_nodes(stk, values, 1);
  stack_free(stk);
}

int main(void) {
  test_new_stack_state();
  test_push_pop_peek();
  test_capacity_limit();
  test_null_stack_guards();
  test_stack_without_backing_list();
  test_invalid_allocators();
  test_stack_allocation_failure();
  test_list_allocation_failure();
  test_first_node_allocation_failure();
  test_second_node_allocation_failure();
  test_unusable_list_allocator();
  test_default_allocator_failures();
  return 0;
}
