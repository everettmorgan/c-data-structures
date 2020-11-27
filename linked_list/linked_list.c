#include<stdio.h>
#include<stdlib.h>

#define SUCCESS  0
#define IS_EMPTY 1
#define IS_FULL  2
#define NOT_FND  3

struct linked_list {
  struct node *h;
  int num_ns;
  int max_sz;
};

struct node {
  int d;
  struct node *n;
  struct node *p;
};

// find : traverses the linked list and return a pointer to the
// first node that matches the data parameter. find() will return
// NULL if no node is found to match the data parameter.
int find(struct linked_list *ll, struct node *ref, int data) {
  struct node *cn = ll->h;
  while (cn->d != data) {
    if (cn->n == NULL) return NOT_FND;
  }
  ref = cn;
  return SUCCESS;
}

// delete_head : removes the head node from the linked list
int delete_head(struct linked_list *ll) {
  if (ll->h == NULL) return IS_EMPTY;
  if (ll->h->n != NULL) {
    ll->h = ll->h->n;
  } else {
    ll->h = NULL;
  }
  return SUCCESS;
}

// delete_by_val : traverses the linked list, matches the data
// parameter against a node in the linked list, and dereferences
// it from it's neighbors. delete_by_val() will return 1 if no
// no matches the data parameter.
int delete_by_val(struct linked_list *ll, int data) {
  if (ll->h == NULL) return IS_EMPTY;

  struct node *prev = NULL;
  struct node *cn = ll->h;
  while (cn->d != data) {
    if (cn->n == NULL) return 1;
    prev = cn;
    cn = cn->n;
  }

  if (cn == ll->h) {
    delete_head(ll);
    return SUCCESS;
  }

  prev->n = cn->n;
  cn->n = NULL;
  return SUCCESS;
}

// insert : inserts a node at the start of a linked list
int insert(struct linked_list *ll, int data) {
  if (ll->max_sz != 0 && ll->num_ns == ll->max_sz)
    return IS_FULL;

  struct node *n = (struct node*)malloc(sizeof(struct node));
  n->d = data;
  n->n = ll->h;
  ll->h = n;
  ll->num_ns++;
  return SUCCESS;
}

// print_linked_list : pretty prints a linked list to the
// console. It will display the node address, it's data propterty
// and the next node's address.
void print_linked_list(struct linked_list *ll) {
  struct node *cn = ll->h;
  while(cn != NULL) {
    printf("addr :%p", cn);
    printf(" data: %d\n", cn->d);
    printf(" next: %p\n", cn->n);
    cn = cn->n;
  }

  printf("\n");
}