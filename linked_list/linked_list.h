/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "linked_list.c"

// meh
#define LL_EQ(name,typa,typb,comp) \
  int name##_equality(void * __a, void * __b) { \
    typa * _a = __a; typb * _b = __b;  \
    return (comp) ? 1 : 0; \
  } \

#define LL_DEL_HEAD(ll) linked_list_delete((linked_list *)ll, (node *)ll->head)
#define LL_DEL_TAIL(ll) linked_list_delete((linked_list *)ll, (node *)ll->tail)
#define LL_INSERT(ll, v) linked_list_insert((linked_list *)ll, linked_list_node_new((void *)v))

linked_list *linked_list_new(int type);
node *linked_list_node_new(void *d);

node *linked_list_find(linked_list *ll, void *v);
void linked_list_insert(linked_list *ll, node *n);
int linked_list_delete(linked_list *ll, node *n);
int linked_list_delete_value(linked_list *ll, void *v);

void linked_list_compare_fn(linked_list *ll, void(*cmp));
void linked_list_equality_fn(linked_list *ll, void(*equal));

void linked_list_free(linked_list *ll);
void linked_list_print(linked_list *ll);
