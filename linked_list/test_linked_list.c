#include <assert.h>

#include "linked_list.h"

static int int_equal(void *left, void *right) {
  return *(int *)left == *(int *)right;
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

int main(void) {
  test_missing_delete(0);
  test_missing_delete(1);
  test_missing_delete(2);
  test_single_node_delete(1);
  test_single_node_delete(2);
  return 0;
}
