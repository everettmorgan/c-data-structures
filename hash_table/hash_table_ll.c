/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "../linked_list/linked_list.h"

#define SIZE 25

typedef void * hash_table[SIZE];

typedef struct hash_tbl_node {
  void * data;
  int key;
} hnode;

hash_table * hash_table_new() {
  hash_table * ht = calloc(SIZE, sizeof(void *));
  return ht;
}

LL_EQ(hnode_key, hnode, int, (_a->key == *_b));

void hash_table_insert(hash_table * ht, void * d, int key) {
    int ll = key % (SIZE-1);
    if ((*ht)[ll] == NULL) {
      (*ht)[ll] = linked_list_new(0);
      linked_list_equality_fn((*ht)[ll], hnode_key_equality);
    }
    hnode * hn = malloc(sizeof(hnode));
    hn->data = d;
    hn->key = key;
    LL_INSERT((*ht)[ll], hn);
}

hnode * hash_table_search(hash_table * ht, int key) {
  int ll = key % (SIZE-1);
  if ((*ht)[ll] != NULL) {
      node * n = linked_list_find((*ht)[ll], &key);
      if (n != NULL) {
          return (hnode *)(n->data);
      }
  }
  return NULL;
}

void hash_table_delete(hash_table * ht, int key) {
    int ll = key % (SIZE-1);
    if ((*ht)[ll] != NULL) {
        node * n = linked_list_find((*ht)[ll], &key);
        if (n != NULL) {
            linked_list_delete((*ht)[ll], n);
        }
    }
}

void hash_table_print(hash_table * ht) {
  for (int i = 0; i < SIZE; i++) {
    if ((*ht)[i] != NULL) {
        printf("--++--\n");
        printf("len: %d\n", ((linked_list *)((*ht)[i]))->length);
        printf("%p\n", (*ht)[i]);
        linked_list_print((*ht)[i]);
    }
  }
}