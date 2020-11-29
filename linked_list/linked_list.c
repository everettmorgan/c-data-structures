/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include<stdio.h>
#include<stdlib.h>

#define SUCCESS  0
#define IS_EMPTY 1
#define IS_FULL  2
#define NOT_FND  3

typedef int linked_list_type;

const linked_list_type SING_LL = 0;
const linked_list_type DOUB_LL = 1;
const linked_list_type CIRC_LL = 2;

// linked_list: a collection of elements
// @h : pointer to the head node in the list
// @num_ns : number of nodes in the list
// @max_sz : (optional) defines a max size for the list, used with stacks
typedef struct linked_list {
  struct node *h;
  int num_ns;
  int max_sz;
  linked_list_type typ;
} linked_list;

// new_linked_list : returns a pointer to a newly allocated linked list.
linked_list *new_linked_list(linked_list_type lt, int max_size) {
  linked_list *ll = (linked_list *)malloc(sizeof(linked_list));
  if (max_size > 0) ll->max_sz = max_size;
  if (lt > 0) ll->typ = lt;
  return ll;
}

// node : an element in a linked list
// @d : stored data
// @n : pointer to the next node
// @p : pointer to the previous node
typedef struct node {
  int d;
  struct node *n;
  struct node *p;
} node;

// new_node : returns a pointer to a newly allocated node.
node *new_node() {
  node *n = (node *)malloc(sizeof(node));
  return n;
}

// find : traverses the linked list and return a pointer to the
// first node that matches the data parameter. find() will return
// NULL if no node is found to match the data parameter.
static node *find(linked_list *ll, int data) {
  node *cn = ll->h;
  while (cn->d != data) {
    if (cn->n == NULL) return NULL;
    cn = cn->n;
  }
  return cn;
}

// delete_head : removes the head node from the linked list
static int delete_head(linked_list *ll) {
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
static int delete_by_val(linked_list *ll, int data) {
  if (ll->h == NULL) return IS_EMPTY;

  node *prev = NULL;
  node *cn = ll->h;
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

// singly_insert : handles inserting into a singly_linked_list
static void _singly_insert(linked_list *ll, int data) {
  node *n = (node*)malloc(sizeof(node));
  n->d = data;
  n->n = ll->h;
  ll->h = n;
  ll->num_ns++;
}

// doubly_insert : handles inserting into a doubly_linked_list
static void _doubly_insert(linked_list *ll, int data) {

}

// circular : handles inserting into a circular_linked_list
static void _circular_insert(linked_list *ll, int data) {

}

// insert : inserts a node at the start of a linked list
static int insert(linked_list *ll, int data) {
  if (ll->max_sz != 0 && ll->num_ns == ll->max_sz)
    return IS_FULL;

  switch (ll->typ) {
    case SING_LL:
      _singly_insert(ll, data);
      break;
    case DOUB_LL:
      _doubly_insert(ll, data);
      break;
    case CIRC_LL:
      _circular_insert(ll, data);
      break;
    default: // invalid: default to singly
      _singly_insert(ll, data);
      break;
  }

  return SUCCESS;
}

// print_linked_list : pretty prints a linked list to the
// console. It will display the node address, it's data propterty
// and the next node's address.
void print_linked_list(linked_list *ll) {
  node *cn = ll->h;
  while(cn != NULL) {
    printf("addr :%p", cn);
    printf(" data: %d\n", cn->d);
    printf(" next: %p\n", cn->n);
    cn = cn->n;
  }

  printf("\n");
}

// free_linked_list : frees all nodes and the parent linked list from the heap.
void free_linked_list(linked_list *ll) {
  node *cn = ll->h;
  while(cn != NULL) {
    node *next = cn->n;
    free(cn);
    cn = next;
  }
  free(ll);
}