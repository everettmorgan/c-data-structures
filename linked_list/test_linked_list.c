#include <assert.h>
#include <limits.h>
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
#include "linked_list.h"
#include "linked_list.h"
#undef malloc

_Static_assert(LINKED_LIST_SINGLY == 0, "singly list type must remain 0");
_Static_assert(LINKED_LIST_DOUBLY == 1, "doubly list type must remain 1");
_Static_assert(LINKED_LIST_CIRCULAR == 2, "circular list type must remain 2");
_Static_assert(LINKED_LIST_OK == 0, "success status must remain 0");
_Static_assert(LINKED_LIST_ERR == 1, "error status must remain 1");

static const linked_list_type list_types[] = {
  LINKED_LIST_SINGLY,
  LINKED_LIST_DOUBLY,
  LINKED_LIST_CIRCULAR
};

enum {
  NO_ALLOCATION_FAILURE = 0,
  LIST_ALLOCATION_ATTEMPT = 1,
  FIRST_NODE_ALLOCATION_ATTEMPT = 2,
  SECOND_NODE_ALLOCATION_ATTEMPT = 3
};

typedef struct allocation_tracker {
  size_t attempts;
  size_t allocations;
  size_t deallocations;
  size_t outstanding;
  size_t fail_on_attempt;
  size_t last_requested_size;
} allocation_tracker;

static int int_equal(void *left, void *right) {
  return *(int *)left == *(int *)right;
}

static int nullable_int_equal(void *left, void *right) {
  if (left == NULL || right == NULL) {
    return left == right;
  }
  return int_equal(left, right);
}

static int compare_ints(void *left, void *right) {
  int left_value = *(int *)left;
  int right_value = *(int *)right;
  return (left_value > right_value) - (left_value < right_value);
}

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

static void assert_list_links(linked_list *list, void *const *values,
                              size_t count) {
  assert(list != NULL);
  assert(list->type == LINKED_LIST_SINGLY ||
         list->type == LINKED_LIST_DOUBLY ||
         list->type == LINKED_LIST_CIRCULAR);
  assert(list->length >= 0);
  assert((size_t)list->length == count);
  assert(linked_list_is_empty(list) == (count == 0));

  if (count == 0) {
    assert(list->head == NULL);
    assert(list->tail == NULL);
    return;
  }

  assert(list->head != NULL);
  assert(list->tail != NULL);
  node *current = list->head;
  node *previous = list->type == LINKED_LIST_CIRCULAR ? list->tail : NULL;
  for (size_t i = 0; i < count; i++) {
    assert(current != NULL);
    assert(current->data == values[i]);
    assert((current == list->tail) == (i + 1 == count));
    if (list->type == LINKED_LIST_SINGLY) {
      assert(current->prev == NULL);
    } else {
      assert(current->prev == previous);
    }
    previous = current;
    current = current->next;
  }
  assert(current == (list->type == LINKED_LIST_CIRCULAR ? list->head : NULL));

  if (list->type == LINKED_LIST_SINGLY) {
    return;
  }
  current = list->tail;
  node *next = list->type == LINKED_LIST_CIRCULAR ? list->head : NULL;
  for (size_t i = count; i > 0; i--) {
    assert(current != NULL);
    assert(current->data == values[i - 1]);
    assert(current->next == next);
    next = current;
    current = current->prev;
  }
  assert(current == (list->type == LINKED_LIST_CIRCULAR ? list->tail : NULL));
}

static void assert_list_unchanged(linked_list *list, node *head, node *tail,
                                  void *const *values, size_t count) {
  assert(list->head == head);
  assert(list->tail == tail);
  assert_list_links(list, values, count);
}

static void *allocate_without_context(size_t size, void *context) {
  assert(context == NULL);
  return malloc(size);
}

static void deallocate_without_context(void *allocation, void *context) {
  assert(context == NULL);
  free(allocation);
}

static void test_missing_delete(int type) {
  int present = 42;
  int missing = 7;
  linked_list *list = linked_list_new(type);
  list->eq_fn = int_equal;
  linked_list_insert(list, &present);

  node outsider = { .data = &missing, .next = NULL, .prev = NULL };
  assert(linked_list_delete(list, NULL) == 1);
  assert(linked_list_delete(list, &outsider) == 1);
  assert(linked_list_delete_value(list, &missing) == 1);
  assert(list->length == 1);
  assert(list->head == list->tail);
  assert(list->head->data == &present);

  assert(linked_list_delete_value(list, &present) == 0);
  assert(list->length == 0);
  assert(list->head == NULL);
  assert(list->tail == NULL);
  linked_list_free(list);
}

static void test_single_node_delete(int type) {
  int value = 42;
  linked_list * list = linked_list_new(type);

  linked_list_insert(list, &value);
  assert(list->head == list->tail);
  assert(linked_list_delete(list, list->head) == 0);
  assert(list->length == 0);
  assert(list->head == NULL);
  assert(list->tail == NULL);

  linked_list_insert(list, &value);
  assert(list->length == 1);
  assert(list->head == list->tail);
  linked_list_free(list);
}

static void test_single_node_links(int type) {
  int value = 42;
  linked_list *list = linked_list_new(type);

  linked_list_insert(list, &value);
  assert(list->length == 1);
  assert(list->head == list->tail);
  assert(list->head->next == NULL);
  assert(list->head->prev == NULL);
  linked_list_free(list);
}

static void test_circular_links(int delete_tail) {
  int first = 42;
  int second = 7;
  linked_list *list = linked_list_new(2);

  linked_list_insert(list, &first);
  assert(list->length == 1);
  assert(list->head == list->tail);
  assert(list->head->next == list->head);
  assert(list->head->prev == list->head);

  linked_list_insert(list, &second);
  assert(list->length == 2);
  assert(list->head != list->tail);
  assert(list->head->data == &first);
  assert(list->tail->data == &second);
  assert(list->head->next == list->tail);
  assert(list->head->prev == list->tail);
  assert(list->tail->next == list->head);
  assert(list->tail->prev == list->head);

  assert(linked_list_delete(list, delete_tail ? list->tail : list->head) == 0);
  assert(list->length == 1);
  assert(list->head == list->tail);
  assert(list->head->data == (delete_tail ? &first : &second));
  assert(list->head->next == list->head);
  assert(list->head->prev == list->head);

  assert(linked_list_delete(list, list->head) == 0);
  assert(list->length == 0);
  assert(list->head == NULL);
  assert(list->tail == NULL);

  linked_list_insert(list, &first);
  assert(list->length == 1);
  assert(list->head == list->tail);
  assert(list->head->data == &first);
  assert(list->head->next == list->head);
  assert(list->head->prev == list->head);
  linked_list_free(list);
}

static void test_invalid_types_and_allocators(void) {
  const int invalid_types[] = { -1, 3, 99, INT_MIN, INT_MAX };
  allocation_tracker tracker = { 0 };
  linked_list_allocator allocator = tracking_allocator(&tracker);
  int value = 1;

  for (size_t i = 0; i < sizeof(invalid_types) / sizeof(invalid_types[0]); i++) {
    assert(linked_list_new(invalid_types[i]) == NULL);
    assert(linked_list_new_with_allocator(invalid_types[i], allocator) == NULL);
  }
  assert_allocation_counts(&tracker, 0, 0, 0);

  const linked_list_allocator invalid_allocators[] = {
    { .allocate = NULL, .deallocate = tracked_deallocate, .context = &tracker },
    { .allocate = tracked_allocate, .deallocate = NULL, .context = &tracker },
    { .allocate = NULL, .deallocate = NULL, .context = &tracker }
  };
  for (size_t i = 0;
       i < sizeof(invalid_allocators) / sizeof(invalid_allocators[0]); i++) {
    for (size_t j = 0; j < sizeof(list_types) / sizeof(list_types[0]); j++) {
      assert(linked_list_new_with_allocator(list_types[j],
                                            invalid_allocators[i]) == NULL);
    }
    assert(linked_list_node_new_with_allocator(&value, &invalid_allocators[i]) == NULL);
  }
  assert(linked_list_node_new_with_allocator(&value, NULL) == NULL);
  assert(linked_list_node_new_with_allocator(NULL, NULL) == NULL);
  assert_allocation_counts(&tracker, 0, 0, 0);
}

static void test_legacy_delete_entry_points(void) {
  int value = 11;
  node outsider = { .data = &value, .next = NULL, .prev = NULL };
  int (*delete_entry_points[])(linked_list *, node *) = {
    linked_list_delete_singly,
    linked_list_delete_doubly,
    linked_list_delete_circly
  };
  _Static_assert(sizeof(delete_entry_points) / sizeof(delete_entry_points[0]) ==
                 sizeof(list_types) / sizeof(list_types[0]),
                 "each list type must have a legacy delete entry point");

  for (size_t i = 0;
       i < sizeof(delete_entry_points) / sizeof(delete_entry_points[0]); i++) {
    assert(delete_entry_points[i](NULL, NULL) == LINKED_LIST_ERR);
    assert(delete_entry_points[i](NULL, &outsider) == LINKED_LIST_ERR);
    linked_list *list = linked_list_new(list_types[i]);
    assert(list != NULL);
    assert(delete_entry_points[i](list, NULL) == LINKED_LIST_ERR);
    assert(delete_entry_points[i](list, &outsider) == LINKED_LIST_ERR);
    assert(linked_list_insert(list, &value) == LINKED_LIST_OK);
    assert(delete_entry_points[i](list, &outsider) == LINKED_LIST_ERR);
    assert(delete_entry_points[i](list, list->head) == LINKED_LIST_OK);
    assert_list_links(list, NULL, 0);
    linked_list_free(list);
  }
}

static void test_null_list_operations(void) {
  int value = 1;
  node outsider = { .data = &value, .next = NULL, .prev = NULL };

  linked_list_free(NULL);
  linked_list_print(NULL);
  assert(linked_list_is_empty(NULL) == 1);
  assert(linked_list_find(NULL, &value) == NULL);
  assert(linked_list_find(NULL, NULL) == NULL);
  assert(linked_list_insert(NULL, &value) == LINKED_LIST_ERR);
  assert(linked_list_insert(NULL, NULL) == LINKED_LIST_ERR);
  assert(linked_list_delete(NULL, &outsider) == LINKED_LIST_ERR);
  assert(linked_list_delete(NULL, NULL) == LINKED_LIST_ERR);
  assert(linked_list_delete_value(NULL, &value) == LINKED_LIST_ERR);
  assert(linked_list_delete_value(NULL, NULL) == LINKED_LIST_ERR);
  linked_list_compare_fn(NULL, NULL);
  linked_list_compare_fn(NULL, (void *)compare_ints);
  linked_list_equality_fn(NULL, NULL);
  linked_list_equality_fn(NULL, (void *)nullable_int_equal);
}

static void test_new_list_state(int type) {
  int value = 1;
  node outsider = { .data = &value, .next = NULL, .prev = NULL };
  linked_list *list = linked_list_new(type);
  assert(list != NULL);
  assert(list->type == type);
  assert(list->cmp_fn == NULL);
  assert(list->eq_fn == NULL);
  assert(list->ops != NULL);
  assert(list->ops->append != NULL);
  assert(list->ops->remove != NULL);
  assert(list->allocator.allocate != NULL);
  assert(list->allocator.deallocate != NULL);
  assert_list_links(list, NULL, 0);
  assert(linked_list_find(list, &value) == NULL);
  assert(linked_list_find(list, NULL) == NULL);
  assert(linked_list_delete(list, NULL) == LINKED_LIST_ERR);
  assert(linked_list_delete(list, &outsider) == LINKED_LIST_ERR);
  assert(linked_list_delete_value(list, &value) == LINKED_LIST_ERR);
  assert(linked_list_delete_value(list, NULL) == LINKED_LIST_ERR);
  linked_list_free(list);

  linked_list_allocator allocator = linked_list_default_allocator();
  list = linked_list_new_with_allocator(type, allocator);
  assert(list != NULL);
  assert_list_links(list, NULL, 0);
  linked_list_free(list);
}

static void test_unset_equality(int type) {
  int first = 11;
  int second = 22;
  void *values[] = { &first, &second };
  linked_list *list = linked_list_new(type);
  assert(list != NULL);
  assert(list->eq_fn == NULL);
  assert(linked_list_insert(list, &first) == LINKED_LIST_OK);
  assert(linked_list_insert(list, &second) == LINKED_LIST_OK);
  node *head = list->head;
  node *tail = list->tail;

  assert(linked_list_find(list, &first) == NULL);
  assert(linked_list_find(list, NULL) == NULL);
  assert(linked_list_delete_value(list, &second) == LINKED_LIST_ERR);
  assert(linked_list_delete_value(list, NULL) == LINKED_LIST_ERR);
  assert_list_unchanged(list, head, tail, values, 2);
  linked_list_free(list);
}

static void test_setters_and_null_payload(int type) {
  int first = 11;
  int equal_to_first = 11;
  int absent = 99;
  void *both[] = { &first, NULL };
  void *null_payload[] = { NULL };
  linked_list *list = linked_list_new(type);
  assert(list != NULL);

  linked_list_compare_fn(list, NULL);
  linked_list_equality_fn(list, NULL);
  assert(list->cmp_fn == NULL);
  assert(list->eq_fn == NULL);
  linked_list_compare_fn(list, (void *)compare_ints);
  linked_list_equality_fn(list, (void *)nullable_int_equal);
  assert(list->cmp_fn == compare_ints);
  assert(list->eq_fn == nullable_int_equal);
  assert(list->cmp_fn(&first, &equal_to_first) == 0);
  assert(list->cmp_fn(&first, &absent) < 0);
  assert(list->eq_fn(&first, &equal_to_first));
  assert(!list->eq_fn(&first, &absent));

  assert(linked_list_insert(list, &first) == LINKED_LIST_OK);
  assert(linked_list_find(list, &equal_to_first) == list->head);
  assert(linked_list_find(list, NULL) == NULL);
  assert(linked_list_delete_value(list, NULL) == LINKED_LIST_ERR);
  assert(linked_list_insert(list, NULL) == LINKED_LIST_OK);
  assert_list_links(list, both, 2);
  assert(linked_list_find(list, &absent) == NULL);
  assert(linked_list_find(list, NULL) == list->tail);
  assert(linked_list_delete_value(list, NULL) == LINKED_LIST_OK);
  assert_list_links(list, both, 1);
  assert(linked_list_insert(list, NULL) == LINKED_LIST_OK);
  assert_list_links(list, both, 2);
  linked_list_print(list);

  assert(linked_list_delete_value(list, &equal_to_first) == LINKED_LIST_OK);
  assert_list_links(list, null_payload, 1);
  linked_list_compare_fn(list, NULL);
  linked_list_equality_fn(list, NULL);
  assert(list->cmp_fn == NULL);
  assert(list->eq_fn == NULL);
  assert(linked_list_find(list, NULL) == NULL);
  assert(linked_list_delete_value(list, NULL) == LINKED_LIST_ERR);
  assert(linked_list_delete(list, list->head) == LINKED_LIST_OK);
  assert_list_links(list, NULL, 0);
  linked_list_free(list);
}

static void test_multi_node_deletions(int type) {
  int values[] = { 10, 20, 30, 40, 50 };
  void *initial[] = { &values[0], &values[1], &values[2], &values[3], &values[4] };
  void *after_middle[] = { &values[0], &values[1], &values[3], &values[4] };
  void *after_head[] = { &values[1], &values[3], &values[4] };
  void *after_tail[] = { &values[1], &values[3] };
  void *last[] = { &values[1] };
  allocation_tracker tracker = { 0 };
  linked_list *list =
      linked_list_new_with_allocator(type, tracking_allocator(&tracker));
  assert(list != NULL);
  linked_list_equality_fn(list, (void *)nullable_int_equal);

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
    assert(linked_list_insert(list, &values[i]) == LINKED_LIST_OK);
    assert_list_links(list, initial, i + 1);
  }
  assert_allocation_counts(&tracker, 6, 6, 0);

  node outsider = { .data = &values[2], .next = list->head, .prev = list->tail };
  node *head = list->head;
  node *tail = list->tail;
  assert(linked_list_delete(list, &outsider) == LINKED_LIST_ERR);
  assert_list_unchanged(list, head, tail, initial, 5);
  assert_allocation_counts(&tracker, 6, 6, 0);

  node *middle = list->head->next->next;
  assert(linked_list_find(list, &values[2]) == middle);
  assert(linked_list_delete(list, middle) == LINKED_LIST_OK);
  assert(linked_list_find(list, &values[2]) == NULL);
  assert_list_links(list, after_middle, 4);
  assert_allocation_counts(&tracker, 6, 6, 1);

  assert(linked_list_delete(list, list->head) == LINKED_LIST_OK);
  assert_list_links(list, after_head, 3);
  assert_allocation_counts(&tracker, 6, 6, 2);
  assert(linked_list_delete(list, list->tail) == LINKED_LIST_OK);
  assert_list_links(list, after_tail, 2);
  assert_allocation_counts(&tracker, 6, 6, 3);
  assert(linked_list_delete_value(list, &values[3]) == LINKED_LIST_OK);
  assert_list_links(list, last, 1);
  assert_allocation_counts(&tracker, 6, 6, 4);
  assert(linked_list_delete(list, list->head) == LINKED_LIST_OK);
  assert_list_links(list, NULL, 0);
  assert(linked_list_delete(list, &outsider) == LINKED_LIST_ERR);
  assert_allocation_counts(&tracker, 6, 6, 5);

  linked_list_free(list);
  assert_allocation_counts(&tracker, 6, 6, 6);
  assert(tracker.outstanding == 0);
}

static void test_invalid_operations(int type) {
  int first = 11;
  int second = 22;
  int third = 33;
  void *values[] = { &first, &second };
  allocation_tracker tracker = { 0 };
  linked_list *list =
      linked_list_new_with_allocator(type, tracking_allocator(&tracker));
  assert(list != NULL);
  const linked_list_ops *valid_ops = list->ops;
  assert(valid_ops != NULL);
  linked_list_ops without_append = *valid_ops;
  linked_list_ops without_remove = *valid_ops;
  without_append.append = NULL;
  without_remove.remove = NULL;

  list->ops = NULL;
  assert(linked_list_insert(list, &first) == LINKED_LIST_ERR);
  assert_list_links(list, NULL, 0);
  assert_allocation_counts(&tracker, 1, 1, 0);
  list->ops = &without_append;
  assert(linked_list_insert(list, &first) == LINKED_LIST_ERR);
  assert_list_links(list, NULL, 0);
  assert_allocation_counts(&tracker, 1, 1, 0);

  list->ops = valid_ops;
  linked_list_equality_fn(list, (void *)nullable_int_equal);
  assert(linked_list_insert(list, &first) == LINKED_LIST_OK);
  assert(linked_list_insert(list, &second) == LINKED_LIST_OK);
  node *head = list->head;
  node *tail = list->tail;
  assert_list_links(list, values, 2);

  list->ops = NULL;
  assert(linked_list_insert(list, &third) == LINKED_LIST_ERR);
  assert(linked_list_delete(list, head) == LINKED_LIST_ERR);
  assert(linked_list_delete_value(list, &first) == LINKED_LIST_ERR);
  assert_list_unchanged(list, head, tail, values, 2);
  assert_allocation_counts(&tracker, 3, 3, 0);

  list->ops = &without_append;
  assert(linked_list_insert(list, &third) == LINKED_LIST_ERR);
  assert(linked_list_delete(list, head) == LINKED_LIST_ERR);
  assert_list_unchanged(list, head, tail, values, 2);
  assert_allocation_counts(&tracker, 3, 3, 0);

  list->ops = &without_remove;
  assert(linked_list_insert(list, &third) == LINKED_LIST_ERR);
  assert(linked_list_delete(list, head) == LINKED_LIST_ERR);
  assert(linked_list_delete_value(list, &first) == LINKED_LIST_ERR);
  assert_list_unchanged(list, head, tail, values, 2);
  assert_allocation_counts(&tracker, 3, 3, 0);

  list->ops = valid_ops;
  list->length = INT_MAX;
  assert(linked_list_insert(list, &third) == LINKED_LIST_ERR);
  assert(list->head == head && list->tail == tail && list->length == INT_MAX);
  list->length = -1;
  assert(linked_list_insert(list, &third) == LINKED_LIST_ERR);
  assert(list->head == head && list->tail == tail && list->length == -1);
  list->length = 2;
  assert_list_unchanged(list, head, tail, values, 2);
  assert_allocation_counts(&tracker, 3, 3, 0);
  linked_list_free(list);
  assert_allocation_counts(&tracker, 3, 3, 3);
  assert(tracker.outstanding == 0);
}

static void test_invalid_list_allocator(int type) {
  int first = 11;
  int second = 22;
  int third = 33;
  void *values[] = { &first, &second };
  allocation_tracker tracker = { 0 };
  linked_list *list =
      linked_list_new_with_allocator(type, tracking_allocator(&tracker));
  assert(list != NULL);
  linked_list_equality_fn(list, (void *)nullable_int_equal);
  assert(linked_list_insert(list, &first) == LINKED_LIST_OK);
  assert(linked_list_insert(list, &second) == LINKED_LIST_OK);
  node *head = list->head;
  node *tail = list->tail;
  linked_list_allocator valid_allocator = list->allocator;

  list->allocator.allocate = NULL;
  assert(linked_list_insert(list, &third) == LINKED_LIST_ERR);
  assert(linked_list_delete(list, head) == LINKED_LIST_ERR);
  assert(linked_list_delete_value(list, &first) == LINKED_LIST_ERR);
  assert_list_unchanged(list, head, tail, values, 2);
  assert_allocation_counts(&tracker, 3, 3, 0);

  list->allocator = valid_allocator;
  list->allocator.deallocate = NULL;
  assert(linked_list_insert(list, &third) == LINKED_LIST_ERR);
  assert(linked_list_delete(list, head) == LINKED_LIST_ERR);
  assert(linked_list_delete_value(list, &first) == LINKED_LIST_ERR);
  linked_list_free(list);
  assert_list_unchanged(list, head, tail, values, 2);
  assert_allocation_counts(&tracker, 3, 3, 0);

  list->allocator = valid_allocator;
  linked_list_free(list);
  assert_allocation_counts(&tracker, 3, 3, 3);
  assert(tracker.outstanding == 0);
}

static void test_default_allocation_failures(void) {
  int first = 11;
  int second = 22;
  void *values[] = { &first };

  reject_default_allocations = 1;
  for (size_t i = 0; i < sizeof(list_types) / sizeof(list_types[0]); i++) {
    assert(linked_list_new(list_types[i]) == NULL);
  }
  assert(linked_list_node_new(&first) == NULL);
  reject_default_allocations = 0;

  for (size_t i = 0; i < sizeof(list_types) / sizeof(list_types[0]); i++) {
    linked_list *list = linked_list_new(list_types[i]);
    assert(list != NULL);

    reject_default_allocations = 1;
    assert(linked_list_insert(list, &first) == LINKED_LIST_ERR);
    reject_default_allocations = 0;
    assert_list_links(list, NULL, 0);

    assert(linked_list_insert(list, &first) == LINKED_LIST_OK);
    node *head = list->head;
    node *tail = list->tail;
    reject_default_allocations = 1;
    assert(linked_list_insert(list, &second) == LINKED_LIST_ERR);
    reject_default_allocations = 0;
    assert_list_unchanged(list, head, tail, values, 1);
    linked_list_free(list);
  }
}

static void test_list_allocation_failure(int type) {
  allocation_tracker tracker = { .fail_on_attempt = LIST_ALLOCATION_ATTEMPT };
  linked_list_allocator allocator = tracking_allocator(&tracker);
  assert(linked_list_new_with_allocator(type, allocator) == NULL);
  assert(tracker.last_requested_size == sizeof(linked_list));
  assert_allocation_counts(&tracker, 1, 0, 0);
  assert(tracker.outstanding == 0);
}

static void test_first_node_allocation_failure(int type) {
  int value = 11;
  void *values[] = { &value };
  allocation_tracker tracker = { .fail_on_attempt = FIRST_NODE_ALLOCATION_ATTEMPT };
  linked_list *list =
      linked_list_new_with_allocator(type, tracking_allocator(&tracker));
  assert(list != NULL);
  assert(tracker.last_requested_size == sizeof(linked_list));
  assert_list_links(list, NULL, 0);
  assert_allocation_counts(&tracker, 1, 1, 0);

  assert(linked_list_insert(list, &value) == LINKED_LIST_ERR);
  assert(tracker.last_requested_size == sizeof(node));
  assert_list_links(list, NULL, 0);
  assert_allocation_counts(&tracker, 2, 1, 0);

  tracker.fail_on_attempt = NO_ALLOCATION_FAILURE;
  assert(linked_list_insert(list, &value) == LINKED_LIST_OK);
  assert_list_links(list, values, 1);
  assert_allocation_counts(&tracker, 3, 2, 0);
  linked_list_free(list);
  assert_allocation_counts(&tracker, 3, 2, 2);
  assert(tracker.outstanding == 0);
}

static void test_second_node_allocation_failure(int type) {
  int first = 11;
  int second = 22;
  void *single[] = { &first };
  void *both[] = { &first, &second };
  void *remaining[] = { &second };
  allocation_tracker tracker = { .fail_on_attempt = SECOND_NODE_ALLOCATION_ATTEMPT };
  linked_list *list =
      linked_list_new_with_allocator(type, tracking_allocator(&tracker));
  assert(list != NULL);
  assert(linked_list_insert(list, &first) == LINKED_LIST_OK);
  assert_list_links(list, single, 1);
  node *head = list->head;
  node *tail = list->tail;
  assert_allocation_counts(&tracker, 2, 2, 0);

  assert(linked_list_insert(list, &second) == LINKED_LIST_ERR);
  assert(tracker.last_requested_size == sizeof(node));
  assert_list_unchanged(list, head, tail, single, 1);
  assert_allocation_counts(&tracker, 3, 2, 0);

  tracker.fail_on_attempt = NO_ALLOCATION_FAILURE;
  assert(linked_list_insert(list, &second) == LINKED_LIST_OK);
  assert_list_links(list, both, 2);
  assert_allocation_counts(&tracker, 4, 3, 0);
  assert(linked_list_delete(list, list->head) == LINKED_LIST_OK);
  assert_list_links(list, remaining, 1);
  assert_allocation_counts(&tracker, 4, 3, 1);
  linked_list_free(list);
  assert_allocation_counts(&tracker, 4, 3, 3);
  assert(tracker.outstanding == 0);
}

static void assert_new_node(node *entry, void *value) {
  assert(entry != NULL);
  assert(entry->data == value);
  assert(entry->next == NULL);
  assert(entry->prev == NULL);
}

static void test_node_allocators(void) {
  int value = 11;
  linked_list_allocator default_allocator = linked_list_default_allocator();
  assert(default_allocator.allocate != NULL);
  assert(default_allocator.deallocate != NULL);

  node *entry = linked_list_node_new(&value);
  assert_new_node(entry, &value);
  free(entry);
  entry = linked_list_node_new(NULL);
  assert_new_node(entry, NULL);
  free(entry);
  entry = linked_list_node_new_with_allocator(&value, &default_allocator);
  assert_new_node(entry, &value);
  default_allocator.deallocate(entry, default_allocator.context);

  allocation_tracker tracker = { .fail_on_attempt = LIST_ALLOCATION_ATTEMPT };
  linked_list_allocator allocator = tracking_allocator(&tracker);
  assert(linked_list_node_new_with_allocator(&value, &allocator) == NULL);
  assert(tracker.last_requested_size == sizeof(node));
  assert_allocation_counts(&tracker, 1, 0, 0);

  tracker.fail_on_attempt = NO_ALLOCATION_FAILURE;
  entry = linked_list_node_new_with_allocator(&value, &allocator);
  assert_new_node(entry, &value);
  allocator.deallocate(entry, allocator.context);
  entry = linked_list_node_new_with_allocator(NULL, &allocator);
  assert_new_node(entry, NULL);
  allocator.deallocate(entry, allocator.context);
  assert_allocation_counts(&tracker, 3, 2, 2);
  assert(tracker.outstanding == 0);
}

static void test_null_allocator_context(int type) {
  int value = 11;
  linked_list_allocator allocator = {
    .allocate = allocate_without_context,
    .deallocate = deallocate_without_context,
    .context = NULL
  };
  linked_list *list = linked_list_new_with_allocator(type, allocator);
  assert(list != NULL);
  linked_list_status status = linked_list_insert(list, &value);
  assert(status == LINKED_LIST_OK);
  linked_list_free(list);

  node *entry = linked_list_node_new_with_allocator(&value, &allocator);
  assert_new_node(entry, &value);
  allocator.deallocate(entry, allocator.context);
}

static void test_allocator_context_isolation(int type) {
  int first_value = 11;
  int second_value = 22;
  allocation_tracker first_tracker = { 0 };
  allocation_tracker second_tracker = { 0 };
  linked_list *first =
      linked_list_new_with_allocator(type, tracking_allocator(&first_tracker));
  linked_list *second =
      linked_list_new_with_allocator(type, tracking_allocator(&second_tracker));
  assert(first != NULL);
  assert(second != NULL);
  assert_allocation_counts(&first_tracker, 1, 1, 0);
  assert_allocation_counts(&second_tracker, 1, 1, 0);

  assert(linked_list_insert(first, &first_value) == LINKED_LIST_OK);
  assert_allocation_counts(&first_tracker, 2, 2, 0);
  assert_allocation_counts(&second_tracker, 1, 1, 0);
  assert(linked_list_insert(second, &second_value) == LINKED_LIST_OK);
  assert_allocation_counts(&first_tracker, 2, 2, 0);
  assert_allocation_counts(&second_tracker, 2, 2, 0);

  assert(linked_list_delete(first, first->head) == LINKED_LIST_OK);
  assert_allocation_counts(&first_tracker, 2, 2, 1);
  assert_allocation_counts(&second_tracker, 2, 2, 0);
  linked_list_free(second);
  assert_allocation_counts(&first_tracker, 2, 2, 1);
  assert_allocation_counts(&second_tracker, 2, 2, 2);
  linked_list_free(first);
  assert_allocation_counts(&first_tracker, 2, 2, 2);
  assert(first_tracker.outstanding == 0);
  assert(second_tracker.outstanding == 0);
}

#ifndef LL_DISABLE_ALIASES
static void test_legacy_aliases(void) {
  int value = 11;
  linked_list *lists[] = { singly_ll, doubly_ll, circly_ll };
  for (size_t i = 0; i < sizeof(lists) / sizeof(lists[0]); i++) {
    linked_list *list = lists[i];
    assert(list != NULL);
    assert(list->type == (int)list_types[i]);
    ll_eq_fn(list, int_equal);
    assert(ll_insert(list, &value) == LINKED_LIST_OK);
    assert(ll_find(list, &value) == list->head);
    assert(ll_del_v(list, &value) == LINKED_LIST_OK);
    ll_free(list);
  }
}
#endif

int main(void) {
  test_missing_delete(0);
  test_missing_delete(1);
  test_missing_delete(2);
  test_single_node_delete(0);
  test_single_node_delete(1);
  test_single_node_delete(2);
  test_single_node_links(0);
  test_single_node_links(1);
  test_circular_links(0);
  test_circular_links(1);

  test_invalid_types_and_allocators();
  test_legacy_delete_entry_points();
  test_null_list_operations();
  test_node_allocators();
  test_default_allocation_failures();
  for (size_t i = 0; i < sizeof(list_types) / sizeof(list_types[0]); i++) {
    int type = list_types[i];
    test_new_list_state(type);
    test_unset_equality(type);
    test_setters_and_null_payload(type);
    test_multi_node_deletions(type);
    test_invalid_operations(type);
    test_invalid_list_allocator(type);
    test_list_allocation_failure(type);
    test_first_node_allocation_failure(type);
    test_second_node_allocation_failure(type);
    test_null_allocator_context(type);
    test_allocator_context_isolation(type);
  }
#ifndef LL_DISABLE_ALIASES
  test_legacy_aliases();
#endif
  return 0;
}
