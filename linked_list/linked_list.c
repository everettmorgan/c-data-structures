/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum linked_list_type {
  LINKED_LIST_SINGLY = 0,
  LINKED_LIST_DOUBLY = 1,
  LINKED_LIST_CIRCULAR = 2
} linked_list_type;

typedef enum linked_list_status {
  LINKED_LIST_OK = 0,
  LINKED_LIST_ERR = 1
} linked_list_status;

/* The allocator's context must outlive the list and its nodes. */
typedef struct linked_list_allocator {
  void *(*allocate)(size_t size, void *context);
  void (*deallocate)(void *ptr, void *context);
  void *context;
} linked_list_allocator;

/* The list owns its nodes, but not the values stored in them. */
typedef struct node {
  void *data;
  struct node *next;
  struct node *prev;
} node;

struct linked_list;

typedef struct linked_list_ops {
  void (*append)(struct linked_list *ll, node *n);
  int (*remove)(struct linked_list *ll, node *n);
} linked_list_ops;

typedef struct linked_list {
  struct node *head;
  struct node *tail;
  int length;
  int type;
  int (*cmp_fn)(void *, void *);
  int (*eq_fn)(void *, void *);
  linked_list_allocator allocator;
  const linked_list_ops *ops;
} linked_list;

static void *linked_list_stdlib_allocate(size_t size, void *context) {
  (void)context;
  return malloc(size);
}

static void linked_list_stdlib_deallocate(void *ptr, void *context) {
  (void)context;
  free(ptr);
}

linked_list_allocator linked_list_default_allocator(void) {
  linked_list_allocator allocator = {
    linked_list_stdlib_allocate,
    linked_list_stdlib_deallocate,
    NULL
  };
  return allocator;
}

static int linked_list_allocator_is_valid(const linked_list_allocator *allocator) {
  return allocator != NULL && allocator->allocate != NULL &&
         allocator->deallocate != NULL;
}

static int linked_list_ops_are_valid(const linked_list_ops *ops) {
  return ops != NULL && ops->append != NULL && ops->remove != NULL;
}

static void linked_list_append_singly(linked_list *ll, node *n) {
  if (ll->tail == NULL) {
    ll->head = n;
  } else {
    ll->tail->next = n;
  }
  ll->tail = n;
}

static void linked_list_append_doubly(linked_list *ll, node *n) {
  n->prev = ll->tail;
  linked_list_append_singly(ll, n);
}

static void linked_list_append_circular(linked_list *ll, node *n) {
  linked_list_append_doubly(ll, n);
  ll->tail->next = ll->head;
  ll->head->prev = ll->tail;
}

static int linked_list_release_node(linked_list *ll, node *n) {
  ll->length -= 1;
  ll->allocator.deallocate(n, ll->allocator.context);
  return LINKED_LIST_OK;
}

static int linked_list_remove_singly(linked_list *ll, node *n) {
  node *previous = NULL;
  node *current = ll->head;
  for (int i = 0; i < ll->length && current != NULL; i++) {
    if (current == n) {
      if (previous == NULL) {
        ll->head = current->next;
      } else {
        previous->next = current->next;
      }
      if (current == ll->tail) {
        ll->tail = previous;
      }
      return linked_list_release_node(ll, current);
    }
    previous = current;
    current = current->next;
  }
  return LINKED_LIST_ERR;
}

static int linked_list_remove_doubly(linked_list *ll, node *n) {
  node *current = ll->head;
  for (int i = 0; i < ll->length && current != NULL; i++) {
    if (current == n) {
      if (current == ll->head) {
        ll->head = current->next;
      } else {
        current->prev->next = current->next;
      }
      if (current == ll->tail) {
        ll->tail = current->prev;
      } else {
        current->next->prev = current->prev;
      }
      return linked_list_release_node(ll, current);
    }
    current = current->next;
  }
  return LINKED_LIST_ERR;
}

static int linked_list_remove_circular(linked_list *ll, node *n) {
  node *current = ll->head;
  for (int i = 0; i < ll->length && current != NULL; i++) {
    if (current == n) {
      if (ll->length == 1) {
        ll->head = NULL;
        ll->tail = NULL;
      } else {
        current->prev->next = current->next;
        current->next->prev = current->prev;
        if (current == ll->head) {
          ll->head = current->next;
        }
        if (current == ll->tail) {
          ll->tail = current->prev;
        }
      }
      return linked_list_release_node(ll, current);
    }
    current = current->next;
  }
  return LINKED_LIST_ERR;
}

static const linked_list_ops linked_list_singly_ops = {
  linked_list_append_singly,
  linked_list_remove_singly
};

static const linked_list_ops linked_list_doubly_ops = {
  linked_list_append_doubly,
  linked_list_remove_doubly
};

static const linked_list_ops linked_list_circular_ops = {
  linked_list_append_circular,
  linked_list_remove_circular
};

static const linked_list_ops *linked_list_ops_for_type(int type) {
  switch (type) {
    case LINKED_LIST_SINGLY:
      return &linked_list_singly_ops;
    case LINKED_LIST_DOUBLY:
      return &linked_list_doubly_ops;
    case LINKED_LIST_CIRCULAR:
      return &linked_list_circular_ops;
    default:
      return NULL;
  }
}

linked_list *linked_list_new_with_allocator(int type,
                                             linked_list_allocator allocator) {
  const linked_list_ops *ops = linked_list_ops_for_type(type);
  if (ops == NULL || !linked_list_allocator_is_valid(&allocator)) {
    return NULL;
  }

  linked_list *ll = allocator.allocate(sizeof(*ll), allocator.context);
  if (ll == NULL) {
    return NULL;
  }

  ll->head = NULL;
  ll->tail = NULL;
  ll->length = 0;
  ll->type = type;
  ll->cmp_fn = NULL;
  ll->eq_fn = NULL;
  ll->allocator = allocator;
  ll->ops = ops;
  return ll;
}

linked_list *linked_list_new(int type) {
  return linked_list_new_with_allocator(type, linked_list_default_allocator());
}

/* Standalone nodes must use their creator's deallocator; list-owned nodes must
 * use the owning list's allocator. */
node *linked_list_node_new_with_allocator(void *d,
                                         const linked_list_allocator *allocator) {
  if (!linked_list_allocator_is_valid(allocator)) {
    return NULL;
  }

  node *n = allocator->allocate(sizeof(*n), allocator->context);
  if (n == NULL) {
    return NULL;
  }
  n->data = d;
  n->next = NULL;
  n->prev = NULL;
  return n;
}

node *linked_list_node_new(void *d) {
  linked_list_allocator allocator = linked_list_default_allocator();
  return linked_list_node_new_with_allocator(d, &allocator);
}

void linked_list_free(linked_list *ll) {
  if (ll == NULL || ll->allocator.deallocate == NULL) {
    return;
  }

  void (*deallocate)(void *, void *) = ll->allocator.deallocate;
  void *context = ll->allocator.context;
  node *current = ll->head;
  for (int i = 0; i < ll->length && current != NULL; i++) {
    node *next = current->next;
    deallocate(current, context);
    current = next;
  }
  deallocate(ll, context);
}

/* A NULL list is considered empty. */
int linked_list_is_empty(linked_list *ll) {
  return ll == NULL || ll->length < 1;
}

/* Equality callbacks receive stored and requested values unchanged, even NULL. */
node *linked_list_find(linked_list *ll, void *v) {
  if (ll == NULL || ll->eq_fn == NULL) {
    return NULL;
  }

  node *current = ll->head;
  for (int i = 0; i < ll->length && current != NULL; i++) {
    if (ll->eq_fn(current->data, v)) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

int linked_list_insert(linked_list *ll, void *d) {
  if (ll == NULL || !linked_list_ops_are_valid(ll->ops) ||
      ll->length < 0 || ll->length == INT_MAX) {
    return LINKED_LIST_ERR;
  }

  node *n = linked_list_node_new_with_allocator(d, &ll->allocator);
  if (n == NULL) {
    return LINKED_LIST_ERR;
  }

  ll->ops->append(ll, n);
  ll->length += 1;
  return LINKED_LIST_OK;
}

int linked_list_delete(linked_list *ll, node *n) {
  if (ll == NULL || n == NULL || linked_list_is_empty(ll) ||
      !linked_list_ops_are_valid(ll->ops) ||
      !linked_list_allocator_is_valid(&ll->allocator)) {
    return LINKED_LIST_ERR;
  }
  return ll->ops->remove(ll, n);
}

/* Preserve the original type-specific entry points for existing callers. */
int linked_list_delete_singly(linked_list *ll, node *n) {
  return linked_list_delete(ll, n);
}

int linked_list_delete_doubly(linked_list *ll, node *n) {
  return linked_list_delete(ll, n);
}

int linked_list_delete_circly(linked_list *ll, node *n) {
  return linked_list_delete(ll, n);
}

int linked_list_delete_value(linked_list *ll, void *v) {
  if (ll == NULL || ll->eq_fn == NULL) {
    return LINKED_LIST_ERR;
  }
  node *n = linked_list_find(ll, v);
  if (n == NULL) {
    return LINKED_LIST_ERR;
  }
  return linked_list_delete(ll, n);
}

/* The void-pointer callback parameters are retained for existing callers. */
void linked_list_compare_fn(linked_list *ll, void *cmp) {
  if (ll != NULL) {
    ll->cmp_fn = cmp;
  }
}

void linked_list_equality_fn(linked_list *ll, void *equal) {
  if (ll != NULL) {
    ll->eq_fn = equal;
  }
}

void linked_list_print(linked_list *ll) {
  if (ll == NULL) {
    return;
  }

  node *current = ll->head;
  for (int i = 0; i < ll->length && current != NULL; i++) {
    printf("addr :%p", (void *)current);
    printf("\tnext: %p", (void *)current->next);
    printf("\tprev: %p", (void *)current->prev);
    printf("\tnull: %d\n", current->data == NULL);
    current = current->next;
  }
  printf("**++**\n");
}
