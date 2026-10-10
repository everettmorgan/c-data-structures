#include <assert.h>

#include "linked_list.h"

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
  test_single_node_delete(1);
  test_single_node_delete(2);
  return 0;
}
