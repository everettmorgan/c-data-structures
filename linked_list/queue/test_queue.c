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
#include "queue.h"
#undef malloc

enum {
  NO_ALLOCATION_FAILURE = 0,
  QUEUE_ALLOCATION_ATTEMPT = 1,
  STACK_ALLOCATION_ATTEMPT = 2,
  LIST_ALLOCATION_ATTEMPT = 3,
  FIRST_NODE_ALLOCATION_ATTEMPT = 4,
  SECOND_NODE_ALLOCATION_ATTEMPT = 5
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

/* Values are listed from the front of the queue to the back. */
static void assert_queue_nodes(const queue *q, void *const *values,
                               size_t count) {
  assert(q != NULL);
  assert(q->stk != NULL);
  assert(q->stk->ll != NULL);
  assert(q->stk->ll->length >= 0);
  assert((size_t)q->stk->ll->length == count);

  if (count == 0) {
    assert(q->stk->ll->head == NULL);
    assert(q->stk->ll->tail == NULL);
    return;
  }

  assert(q->stk->ll->head != NULL);
  assert(q->stk->ll->tail != NULL);
  node *current = q->stk->ll->head;
  for (size_t i = 0; i < count; i++) {
    assert(current != NULL);
    assert(current->data == values[i]);
    assert(current->prev == NULL);
    assert((current == q->stk->ll->tail) == (i + 1 == count));
    current = current->next;
  }
  assert(current == NULL);
}

static void assert_queue_unchanged(const queue *q, node *head, node *tail,
                                   void *const *values, size_t count) {
  assert(q->stk->ll->head == head);
  assert(q->stk->ll->tail == tail);
  assert_queue_nodes(q, values, count);
}

static void test_fifo_order(void) {
  int first = 1;
  int second = 2;
  int third = 3;
  queue *q = queue_new(3);
  assert(q != NULL);

  assert(queue_enqueue(q, &first) == 0);
  assert(queue_enqueue(q, &second) == 0);
  assert(queue_enqueue(q, &third) == 0);
  assert(queue_peek(q)->data == &first);

  assert(queue_dequeue(q) == 0);
  assert(queue_peek(q)->data == &second);
  assert(queue_dequeue(q) == 0);
  assert(queue_peek(q)->data == &third);
  assert(queue_dequeue(q) == 0);
  assert(queue_peek(q) == NULL);
  assert(queue_dequeue(q) == 1);

  queue_free(q);
}

static void test_new_queue_state(void) {
  allocation_tracker tracker = { 0 };
  queue *q = queue_new_with_allocator(2, tracking_allocator(&tracker));

  assert(q != NULL);
  assert(q->allocator.allocate == tracked_allocate);
  assert(q->allocator.deallocate == tracked_deallocate);
  assert(q->allocator.context == &tracker);
  assert(q->stk != NULL);
  assert(q->stk->max_length == 2);
  assert(q->stk->allocator.context == &tracker);
  assert(q->stk->ll->allocator.context == &tracker);
  assert_queue_nodes(q, NULL, 0);
  assert_allocation_counts(&tracker, 3, 3, 0);

  queue_free(q);
  assert_allocation_counts(&tracker, 3, 3, 3);
}

static void test_capacity_limit(void) {
  int first = 1;
  int second = 2;
  int rejected = 3;
  void *full[] = { &first, &second };
  allocation_tracker tracker = { 0 };
  queue *q = queue_new_with_allocator(2, tracking_allocator(&tracker));
  assert(q != NULL);

  assert(queue_enqueue(q, &first) == 0);
  assert(queue_enqueue(q, &second) == 0);
  assert_allocation_counts(&tracker, 5, 5, 0);
  node *head = q->stk->ll->head;
  node *tail = q->stk->ll->tail;

  assert(queue_enqueue(q, &rejected) == 1);
  assert_queue_unchanged(q, head, tail, full, 2);
  assert_allocation_counts(&tracker, 5, 5, 0);

  queue_free(q);
  assert_allocation_counts(&tracker, 5, 5, 5);
}

static void test_null_queue_guards(void) {
  int value = 1;

  queue_free(NULL);
  queue_print(NULL);
  assert(queue_enqueue(NULL, &value) == 1);
  assert(queue_enqueue(NULL, NULL) == 1);
  assert(queue_dequeue(NULL) == 1);
  assert(queue_peek(NULL) == NULL);
}

static void test_queue_without_backing_stack(void) {
  int value = 1;
  allocation_tracker tracker = { 0 };
  queue detached = {
    .stk = NULL,
    .allocator = tracking_allocator(&tracker)
  };

  assert(queue_enqueue(&detached, &value) == 1);
  assert(queue_dequeue(&detached) == 1);
  assert(queue_peek(&detached) == NULL);
  queue_print(&detached);
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
    assert(queue_new_with_allocator(4, invalid_allocators[i]) == NULL);
  }
  assert_allocation_counts(&tracker, 0, 0, 0);
}

static void test_queue_allocation_failure(void) {
  allocation_tracker tracker = { .fail_on_attempt = QUEUE_ALLOCATION_ATTEMPT };

  assert(queue_new_with_allocator(4, tracking_allocator(&tracker)) == NULL);
  assert(tracker.last_requested_size == sizeof(queue));
  assert_allocation_counts(&tracker, 1, 0, 0);
}

static void test_stack_allocation_failure(void) {
  allocation_tracker tracker = { .fail_on_attempt = STACK_ALLOCATION_ATTEMPT };

  assert(queue_new_with_allocator(4, tracking_allocator(&tracker)) == NULL);
  assert(tracker.last_requested_size == sizeof(stack));
  assert_allocation_counts(&tracker, 2, 1, 1);
  assert(tracker.outstanding == 0);
}

static void test_list_allocation_failure(void) {
  allocation_tracker tracker = { .fail_on_attempt = LIST_ALLOCATION_ATTEMPT };

  assert(queue_new_with_allocator(4, tracking_allocator(&tracker)) == NULL);
  assert(tracker.last_requested_size == sizeof(linked_list));
  assert_allocation_counts(&tracker, 3, 2, 2);
  assert(tracker.outstanding == 0);
}

static void test_first_node_allocation_failure(void) {
  int value = 11;
  void *values[] = { &value };
  allocation_tracker tracker = { .fail_on_attempt = FIRST_NODE_ALLOCATION_ATTEMPT };
  queue *q = queue_new_with_allocator(4, tracking_allocator(&tracker));
  assert(q != NULL);
  assert_allocation_counts(&tracker, 3, 3, 0);

  assert(queue_enqueue(q, &value) == 1);
  assert(tracker.last_requested_size == sizeof(node));
  assert_queue_nodes(q, NULL, 0);
  assert_allocation_counts(&tracker, 4, 3, 0);

  tracker.fail_on_attempt = NO_ALLOCATION_FAILURE;
  assert(queue_enqueue(q, &value) == 0);
  assert_queue_nodes(q, values, 1);
  assert_allocation_counts(&tracker, 5, 4, 0);

  queue_free(q);
  assert_allocation_counts(&tracker, 5, 4, 4);
  assert(tracker.outstanding == 0);
}

static void test_second_node_allocation_failure(void) {
  int first = 11;
  int second = 22;
  void *single[] = { &first };
  void *both[] = { &first, &second };
  void *remaining[] = { &second };
  allocation_tracker tracker = { .fail_on_attempt = SECOND_NODE_ALLOCATION_ATTEMPT };
  queue *q = queue_new_with_allocator(4, tracking_allocator(&tracker));
  assert(q != NULL);
  assert(queue_enqueue(q, &first) == 0);
  assert_queue_nodes(q, single, 1);
  node *head = q->stk->ll->head;
  node *tail = q->stk->ll->tail;
  assert_allocation_counts(&tracker, 4, 4, 0);

  assert(queue_enqueue(q, &second) == 1);
  assert(tracker.last_requested_size == sizeof(node));
  assert_queue_unchanged(q, head, tail, single, 1);
  assert_allocation_counts(&tracker, 5, 4, 0);

  tracker.fail_on_attempt = NO_ALLOCATION_FAILURE;
  assert(queue_enqueue(q, &second) == 0);
  assert_queue_nodes(q, both, 2);
  assert_allocation_counts(&tracker, 6, 5, 0);

  assert(queue_dequeue(q) == 0);
  assert_queue_nodes(q, remaining, 1);
  assert_allocation_counts(&tracker, 6, 5, 1);

  queue_free(q);
  assert_allocation_counts(&tracker, 6, 5, 5);
  assert(tracker.outstanding == 0);
}

static void test_unusable_list_allocator(void) {
  int value = 11;
  void *values[] = { &value };
  allocation_tracker tracker = { 0 };
  queue *q = queue_new_with_allocator(4, tracking_allocator(&tracker));
  assert(q != NULL);
  linked_list_allocator valid_allocator = q->stk->ll->allocator;

  q->stk->ll->allocator.allocate = NULL;
  assert(queue_enqueue(q, &value) == 1);
  assert_queue_nodes(q, NULL, 0);

  q->stk->ll->allocator = valid_allocator;
  assert(queue_enqueue(q, &value) == 0);
  assert_queue_nodes(q, values, 1);
  q->stk->ll->allocator.allocate = NULL;
  assert(queue_enqueue(q, &value) == 1);
  assert_queue_nodes(q, values, 1);
  assert_allocation_counts(&tracker, 4, 4, 0);

  q->stk->ll->allocator = valid_allocator;
  queue_free(q);
  assert_allocation_counts(&tracker, 4, 4, 4);
}

static void test_default_allocator_failures(void) {
  int first = 11;
  int second = 22;
  void *single[] = { &first };

  reject_default_allocations = 1;
  assert(queue_new(4) == NULL);
  reject_default_allocations = 0;

  queue *q = queue_new(4);
  assert(q != NULL);
  reject_default_allocations = 1;
  assert(queue_enqueue(q, &first) == 1);
  reject_default_allocations = 0;
  assert_queue_nodes(q, NULL, 0);

  assert(queue_enqueue(q, &first) == 0);
  assert_queue_nodes(q, single, 1);
  reject_default_allocations = 1;
  assert(queue_enqueue(q, &second) == 1);
  reject_default_allocations = 0;
  assert_queue_nodes(q, single, 1);

  queue_free(q);
}

int main(void) {
  test_fifo_order();
  test_new_queue_state();
  test_capacity_limit();
  test_null_queue_guards();
  test_queue_without_backing_stack();
  test_invalid_allocators();
  test_queue_allocation_failure();
  test_stack_allocation_failure();
  test_list_allocation_failure();
  test_first_node_allocation_failure();
  test_second_node_allocation_failure();
  test_unusable_list_allocator();
  test_default_allocator_failures();
  return 0;
}
