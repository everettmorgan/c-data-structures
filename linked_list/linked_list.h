#include "linked_list.c"

// - Insertion − add an element at the beginning of the list.
// - Deletion − delete an element at the beginning of the list.
// - Display − displaying complete list.
// - Find − find an element using given key.
// - Delete − delete an element using given key.

static int insert(struct linked_list *ll, int data);
static struct node *find(struct linked_list *ll, int data);

static int delete_head(struct linked_list *ll);
static int delete_by_val(struct linked_list *ll, int data);

void print_linked_list(struct linked_list *ll);