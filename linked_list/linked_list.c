/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include<stdio.h> // printf
#include<stdlib.h> // malloc

// linked_list_node : an element in a linked list
typedef struct node {
  void * data;
  struct node * next;
  struct node * prev;
} node;

// linked_list: a collection of nodes
typedef struct linked_list {
  struct node *head;
  struct node *tail;
  int length;
  int type;
  int (* cmp_fn)(void *, void *);
  int (* eq_fn)(void *, void *);
} linked_list;

// linked_list_new : returns a pointer to a newly allocated linked list.
linked_list * linked_list_new(int type) {
  linked_list * ll = malloc(sizeof(linked_list));
  ll->type = type;
  ll->length = 0;
  ll->head = NULL;
  ll->tail = NULL;
  ll->cmp_fn = NULL;
  ll->eq_fn = NULL;
  return ll;
}

// linked_list_node_new : returns a pointer to a newly allocated node.
node * linked_list_node_new(void * d) {
  node * n = malloc(sizeof(node));
  n->data = d;
  n->next = NULL;
  n->prev = NULL;
  return n;
}

// linked_list_free : frees a linked list in it's entirety.
void linked_list_free(linked_list *ll) {
  node * cn = ll->head;
  node * next = NULL;
  int i = 0;
  while (cn != NULL && i < ll->length) {
    next = cn->next;
    free(cn);
    cn = next;
    i++;
  }
  free(ll);
}

/* find_node : traverses a linked list using the user defined check_func()
* function. find_node() expects check_func() to return a 0 for no match
* and any positive integer for a match.
*/
node * linked_list_find(linked_list * ll, void * v) {
  node * curr = ll->head;
  int i = 0;
  while (curr != NULL && i < ll->length) {
    if (ll->eq_fn(curr->data, v)) {
      return curr;
    }
    curr = curr->next;
    i += 1;
  }
  return NULL;
}

// linked_list_insert : inserts a node into a linked list.
void linked_list_insert(linked_list * ll, void * d) {
  node * n = linked_list_node_new(d);

  if (ll->length == 0) {
    ll->head = n;
    ll->tail = n;
  }
  else {
    switch (ll->type) {
    case 0:
      ll->tail->next = n;
      ll->tail = n;
      break;
    case 1:
      ll->tail->next = n;
      n->prev = ll->tail;
      ll->tail = n;
      break;
    case 2:
      ll->tail->next = n;
      n->prev = ll->tail;
      ll->tail = n;
      ll->head->prev = ll->tail;
      ll->tail->next = ll->head;
      break;
    }
  }

  ll->length += 1;
}

/* linked_list_is_empty : returns whether or not a linked list is empty
* or not. is_empty() will return 1 if a linked list is empty and 0 if
* it isn't empty.
*/
int linked_list_is_empty(linked_list * ll) {
  return (ll->length < 1);
}

int linked_list_delete_singly(linked_list * ll, node * n) {
  node * curr = ll->head;
  node * prev = NULL;
  while (curr != NULL) {
    if (&(curr->data) == &(n->data)) {
      if (curr == ll->head) {
        ll->head = curr->next;
      }
      else if (curr == ll->tail) {
        ll->tail = prev;
      }
      else {
        prev->next = curr->next;
        curr->next->prev = prev;
      }
      free(curr);
      ll->length -= 1;
      return 1;
   }
   prev = curr;
   curr = curr->next;
  }
  return 0;
}

int linked_list_delete_doubly(linked_list * ll, node * n) {
  node * curr = ll->head;
  while (curr != NULL) {
    if (&(curr->data) == &(n->data)){
      if (curr == ll->head) {
        ll->head = curr->next;
        if (ll->head != NULL) {
            ll->head->prev = NULL;
        }
      }
      else if (curr == ll->tail) {
        ll->tail = curr->prev;
        ll->tail->next = NULL;
      }
      else {
        if (curr->next != NULL) {
          curr->next->prev = curr->prev;
        }
        if (curr->prev != NULL) {
          curr->prev->next = curr->next;
        }
      }
      free(curr);
      ll->length -= 1;
      return 0;
    }
    curr = curr->next;
  }
  return 1;
}

int linked_list_delete_circly(linked_list * ll, node * n) {
  node * curr = ll->head;
  int i = 0;
  while (curr != NULL && i < ll->length) {
    if (&(curr->data) == &(n->data)) {
      if (curr == ll->head) {
        ll->head = curr->next;
          if (ll->head != NULL) {
              ll->tail->next = ll->head;
              ll->head->prev = ll->tail;
          }
      }
      else if (curr == ll->tail) {
        ll->tail = ll->tail->prev;
        ll->tail->next = ll->head;
        ll->head->prev = ll->tail;
      }
      else {
        curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
      }
      free(curr);
      ll->length -= 1;
      return 0;
    }
    curr = curr->next;
    i += 1;
  }
  return 1;
}

// linked_list_delete : safely removes a node from a linked list.
int linked_list_delete(linked_list * ll, node * n) {
  if (linked_list_is_empty(ll))
    return 1;
  if (ll->type == 0)
    linked_list_delete_singly(ll, n);
  else if (ll->type == 1)
    linked_list_delete_doubly(ll, n);
  else if (ll->type == 2)
    linked_list_delete_circly(ll, n);
  return 0;
}

/* delete_by_val : traverses the linked list, matches the data
* parameter against a node in the linked list, and dereferences
* it from it's neighbors. delete_by_val() will return 1 if no
* no matches the data parameter.
*/
int linked_list_delete_value(linked_list * ll, void * v) {
  if (linked_list_is_empty(ll)) return 1;
  node * n = linked_list_find(ll, v);
  linked_list_delete(ll, n);
  return 0;
}

// linked_list_compare_fn : sets a linked_list's compare function.
void linked_list_compare_fn(linked_list * ll, void (* cmp)) {
    ll->cmp_fn = cmp;
}

// linked_list_equality_fn : sets a linked_list's equality function.
void linked_list_equality_fn(linked_list *ll, void (*equal)) {
    ll->eq_fn = equal;
}

/* linked_list_print : pretty prints a linked list to the console. It will
* display the node address, it's data property and the next node's address.
*/
void linked_list_print(linked_list * ll) {
  node * cn = ll->head;
  int i = 0;
  while (cn != NULL && i < ll->length) {
    printf("addr :%p", cn);
    printf("\tnext: %p", cn->next);
    printf("\tprev: %p", cn->prev);
    printf("\tnull: %d\n", cn->data == NULL);
    cn = cn->next;
    i++;
  }
  printf("**++**\n");
}
