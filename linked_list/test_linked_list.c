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

int main(void) {
  test_missing_delete(0);
  test_missing_delete(1);
  test_missing_delete(2);
  test_single_node_delete(1);
  test_single_node_delete(2);
  test_single_node_links(0);
  test_single_node_links(1);
  test_circular_links(0);
  test_circular_links(1);
  return 0;
}
