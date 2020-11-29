#include "../linked_list.h"

int main() {
  linked_list *ll = new_linked_list(5);
  insert(ll, 'h');
  insert(ll, 'e');
  insert(ll, 'l');
  insert(ll, 'l');
  insert(ll, 'o');

  int full = insert(ll, 'z');
  if (full != IS_FULL)
    printf("ERROR: stack should be full!");

  print_linked_list(ll);

  node *n = find(ll, 'l');
  if (n != NULL)
    printf("found: %d\n", n->d);
  else
    printf("did not find %d\n", 'l');

  delete_by_val(ll, 'y');
  print_linked_list(ll);

  delete_head(ll);
  print_linked_list(ll);

  free_linked_list(ll);
  return 0;
}