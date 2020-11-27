#include "linked_list.c"

// - Insertion − add an element at the beginning of the list.
// - Deletion − delete an element at the beginning of the list.
// - Display − displaying complete list.
// - Find − find an element using given key.
// - Delete − delete an element using given key.

int insert(struct linked_list *ll, int data);
int find(struct linked_list *ll, struct node *ref, int data);

int delete_head(struct linked_list *ll);
int delete_by_val(struct linked_list *ll, int data);

void print_linked_list(struct linked_list *ll);