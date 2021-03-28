/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef LINKED_LIST

#include "linked_list.c"

#ifndef LL_DISABLE_ALIASES

#define singly_ll linked_list_new(0)
#define doubly_ll linked_list_new(1)
#define circly_ll linked_list_new(2)

#ifdef LL_ENABLE_SHRTHND
  #define ll_find(__v) ll_find(ll, __v)

  #define ll_pop() ll_pop(ll)

  #define ll_drop() ll_drop(ll)

  #define ll_insert(__v) ll_insert(ll, __v)

  #define ll_del_v(__v) ll_del_v(ll, __v)

  #define ll_print() ll_print(ll)

  #define ll_free() ll_free(ll)
#endif

#define ll_new(__t) linked_list_new(__t)

// ll_find : find v
#define ll_find(__ll, __v) linked_list_find((linked_list *)(__ll), (void *)(__v))

// ll_pop : remove the HEAD node
#define ll_pop(__ll) linked_list_delete((linked_list *)(__ll), (node *)(__ll)->head)

// ll_drop : remove the TAIL node
#define ll_drop(__ll) linked_list_delete((linked_list *)(__ll), (node *)(__ll)->tail)

// ll_insert : insert v
#define ll_insert(__ll, __v) linked_list_insert((linked_list *)(__ll), (void *)(__v))

// ll_del : remove n
#define ll_del(__ll, __n) linked_list_delete((linked_list *)(__ll), (node *)(__n))

// ll_del_v : remove node by value
#define ll_del_v(__ll, __v) linked_list_delete_value((linked_list *)(__ll), (void *)(__v))

// ll_print : print a linked list
#define ll_print(__ll) linked_list_print((linked_list *)(__ll))

// ll_free : free a linked list
#define ll_free(__ll) linked_list_free((linked_list *)(__ll))

#define ll_eq_fn(__ll, __fn) linked_list_equality_fn(__ll, __fn)

#define ll_prop(__ll, __prop) ((linked_list *)(__ll))->__prop

#else

linked_list *linked_list_new(int type);

node *linked_list_node_new(void * d);

node *linked_list_find(linked_list * ll, void * v);

void linked_list_insert(linked_list * ll, void * d);

int linked_list_delete(linked_list * ll, node * n);

int linked_list_delete_value(linked_list * ll, void * v);

void linked_list_compare_fn(linked_list * ll, void( * cmp ));

void linked_list_equality_fn(linked_list * ll, void( * equal ));

void linked_list_free(linked_list * ll);

void linked_list_print(linked_list * ll);

#endif // LL_DISABLE_ALIASES

#endif // LINKED_LIST