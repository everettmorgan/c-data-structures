#include "linked_list.h"

int main() {
  struct linked_list ll = {};
  insert(&ll, 'h');
  insert(&ll, 'e');
  insert(&ll, 'y');
  insert(&ll, 'y');
  insert(&ll, 'o');
  print_linked_list(&ll);

  struct node *n = NULL;
  find(&ll, 'o', n);
  printf("found: %d\n", n->d);

  delete_by_val(&ll, 'y');
  print_linked_list(&ll);

  delete_head(&ll);
  print_linked_list(&ll);
  return 0;
}