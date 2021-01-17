/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include<stdio.h> // printf
#include<stdlib.h> // malloc

// node : an element in a linked list
typedef struct linked_list_node {
  void * data;
  struct linked_list_node * next;
  struct linked_list_node * prev;
} linked_list_node;

// linked_list: a collection of nodes
typedef struct linked_list {
  struct linked_list_node *head;
  struct linked_list_node *tail;
  int length;
  int type;
  int (* cmp_fn)(void *, void *);
  int (* eq_fn)(void *, void *);
} linked_list;

// new_linked_list : returns a pointer to a newly allocated linked list.
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

// new_node : returns a pointer to a newly allocated node.
linked_list_node * linked_list_node_new(void * d) {
  linked_list_node * n = malloc(sizeof(linked_list_node));
  n->data = d;
  n->next = NULL;
  n->prev = NULL;
  return n;
}

// free_linked_list : frees all nodes and the parent linked list from the stack.
void linked_list_free(linked_list *ll) {
  linked_list_node * cn = ll->head;
  linked_list_node * next = NULL;
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
linked_list_node * linked_list_find(linked_list * ll, void * v) {
  linked_list_node * curr = ll->head;
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

// insert : inserts a node into the linked list.
void linked_list_insert(linked_list * ll, linked_list_node * n) {
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

/* is_empty : returns whether or not a linked list is empty or not.
* is_empty() will return 1 if a linked list is empty and 0 if it isn't.
*/
int linked_list_is_empty(linked_list * ll) {
  return (ll->length < 1);
}

int linked_list_delete_singly(linked_list * ll, linked_list_node * n) {
  linked_list_node * curr = ll->head;
  while (curr != NULL) {
    if (&(curr->data) == &(n->data)) {
      if (curr == ll->head) {
        ll->head = curr->next;
      }
      else if (curr == ll->tail) {
        ll->tail = ll->tail->prev;
        ll->tail->next = NULL;
      }
      else {
        if (curr->next != NULL)
          curr->next->prev = curr->prev;
        if (curr->prev != NULL)
          curr->prev->next = curr->next;
      }
      free(curr);
      ll->length -= 1;
      return 1;
   }
   curr = curr->next;
  }
  return 0;
}

int linked_list_delete_doubly(linked_list * ll, linked_list_node * node) {
  linked_list_node * curr = ll->head;
  while (curr != NULL) {
    if (&(curr->data) == &(node->data)){
      if (curr == ll->head) {
        ll->head = curr->next;
        ll->head->prev = NULL;
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

int linked_list_delete_circly(linked_list * ll, linked_list_node * node) {
  linked_list_node * curr = ll->head;
  int i = 0;
  while (curr != NULL && i < ll->length) {
    if (&(curr->data) == &(node->data)) {
      if (curr == ll->head) {
        ll->head = curr->next;
        ll->tail->next = ll->head;
        ll->head->prev = ll->tail;
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

// delete_node : safely removes a node from it's linked list.
int linked_list_delete(linked_list * ll, linked_list_node * n) {
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
  linked_list_node * node = linked_list_find(ll, v);
  linked_list_delete(ll, node);
  return 0;
}

void linked_list_compare_fn(linked_list * ll, void (* cmp)) {
    ll->cmp_fn = cmp;
}

void linked_list_equality_fn(linked_list *ll, void (*equal)) {
    ll->eq_fn = equal;
}

/* print_linked_list : pretty prints a linked list to the console. It will
* display the node address, it's data propterty and the next node's address.
* */
void linked_list_print(linked_list * ll) {
  linked_list_node * cn = ll->head;
  int i = 0;
  while (cn != NULL && i < ll->length) {
    printf("addr :%p", cn);
    printf("\tnext: %p", cn->next);
    printf("\tprev: %p\n", cn->prev);
    cn = cn->next;
    i++;
  }
  printf("**++**\n");
}
