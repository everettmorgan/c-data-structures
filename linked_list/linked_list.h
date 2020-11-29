#include "linked_list.c"

// - Insertion − add an element at the beginning of the list.
// - Deletion − delete an element at the beginning of the list.
// - Display − displaying complete list.
// - Find − find an element using given key.
// - Delete − delete an element using given key.

linked_list *new_linked_list();
node *new_node();

static int insert(linked_list *ll, int data);
static struct node *find(linked_list *ll, int data);

static int delete_head(linked_list *ll);
static int delete_by_val(linked_list *ll, int data);

void print_linked_list(linked_list *ll);
void free_linked_list(linked_list *ll);