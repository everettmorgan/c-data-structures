#include "../linked_list.h"

int main() {
  linked_list ll = {};
  insert(&ll, 'h');
  insert(&ll, 'e');
  insert(&ll, 'l');
  insert(&ll, 'l');
  insert(&ll, 'o');
  print_linked_list(&ll);

  node *n = find(&ll, 'l');
  if (n != NULL)
    printf("found: %d\n", n->d);

  delete_by_val(&ll, 'y');
  print_linked_list(&ll);

  delete_head(&ll);
  print_linked_list(&ll);
  free_linked_list(&ll);
  return 0;
}