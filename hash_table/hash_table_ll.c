/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef HASH_TABLE_LL
#define HASH_TABLE_LL

#include<stdio.h>
#include<stdlib.h>

#include "../linked_list/linked_list.h"

// prime bucket count: good distribution, every slot reachable
#ifndef HASH_TABLE_SIZE
  #define HASH_TABLE_SIZE 31
#endif

// hash : positive modulo so negative keys map into [0, HASH_TABLE_SIZE)
#define hash(__key) ( ( ( ( (int)(__key) ) % HASH_TABLE_SIZE ) + HASH_TABLE_SIZE ) % HASH_TABLE_SIZE )
#define retrieve(__key) ( ht[ hash( ( ( (int)__key ) ) ) ])

typedef void ** hash_table;

typedef struct hash_tbl_node {
  void * data;
  int key;
} hnode;

int hnode_key_equality(hnode * a, int * b) {
  return a->key == *b;
}

hash_table hash_table_new() {
  return calloc(HASH_TABLE_SIZE, sizeof(void *));
}

// hash_table_insert : stores d under key. If key already exists its
// value is replaced instead of a duplicate entry being appended.
void hash_table_insert(hash_table ht, void * d, int key) {
    int ll = hash(key);
    if (ht[ll] == NULL) {
      ht[ll] = ll_new(0);
      ll_eq_fn(ht[ll], hnode_key_equality);
    }
    else {
      node * existing = ll_find(ht[ll], &key);
      if (existing != NULL) {
        ((hnode *)existing->data)->data = d;
        return;
      }
    }
    hnode * hn = malloc(sizeof(hnode));
    hn->data = d;
    hn->key = key;
    ll_insert(ht[ll], hn);
}

hnode * hash_table_search(hash_table ht, int key) {
  if (retrieve(key)) {
      node * n = ll_find(ht[hash(key)], &key);
      if (n != NULL) {
          return (hnode *)(n->data);
      }
  }
  return NULL;
}

// hash_table_delete : removes key's entry and frees its hnode.
void hash_table_delete(hash_table ht, int key) {
    int ll = hash(key);
    if (ht[ll] != NULL) {
        node * n = ll_find(ht[ll], &key);
        if (n != NULL) {
            free(n->data);
            ll_del(ht[ll], n);
        }
    }
}

// hash_table_free : frees every bucket, hnode and the table itself.
// Caller-owned data is left untouched.
void hash_table_free(hash_table ht) {
  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    if (ht[i] != NULL) {
      node * curr = ((linked_list *)ht[i])->head;
      while (curr != NULL) {
        node * next = curr->next;
        free(curr->data);
        curr = next;
      }
      ll_free(ht[i]);
    }
  }
  free(ht);
}

void hash_table_print(hash_table ht) {
  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    if (ht[i] != NULL) {
        printf("--++--\n");
        printf("len: %d\n", ll_prop(ht[i], length));
        printf("%p\n", ht[i]);
        ll_print(ht[i]);
    }
  }
}
#endif
