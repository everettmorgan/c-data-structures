/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef HASH_TABLE_LL
#define HASH_TABLE_LL

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "../linked_list/linked_list.h"

#ifndef HASH_TABLE_SIZE
  #define HASH_TABLE_SIZE 25
#endif

#define hash(__key) ( ( (int)__key ) % ( ( HASH_TABLE_SIZE - 1 ) ) )
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

void hash_table_insert(hash_table ht, void * d, int key) {
    if (ht == NULL) {
        return;
    }
    int ll = hash(key);
    if (ht[ll] == NULL) {
      ht[ll] = ll_new(0);
      if (ht[ll] == NULL) {
          return;
      }
      ll_eq_fn(ht[ll], hnode_key_equality);
    }
    hnode * hn = malloc(sizeof(hnode));
    if (hn == NULL) {
        return;
    }
    hn->data = d;
    hn->key = key;
    if (ll_insert(ht[ll], hn) != LINKED_LIST_OK) {
        free(hn);
    }
}

hnode * hash_table_search(hash_table ht, int key) {
  if (ht == NULL) {
      return NULL;
  }
  if (retrieve(key)) {
      node * n = ll_find(ht[hash(key)], &key);
      if (n != NULL) {
          return (hnode *)(n->data);
      }
  }
  return NULL;
}

void hash_table_delete(hash_table ht, int key) {
    if (ht == NULL) {
        return;
    }
    int ll = hash(key);
    if (ht[ll] != NULL) {
        node * n = ll_find(ht[ll], &key);
        if (n != NULL) {
            hnode * hn = (hnode *)(n->data);
            /* The node must leave the list before its wrapper is released. */
            if (ll_del(ht[ll], n) == LINKED_LIST_OK) {
                free(hn);
            }
        }
    }
}

void hash_table_free(hash_table ht) {
    if (ht == NULL) {
        return;
    }
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        linked_list * ll = (linked_list *)ht[i];
        if (ll == NULL) {
            continue;
        }
        /* The table owns the hnode wrappers, but not the values they point at. */
        node * current = ll->head;
        for (int j = 0; j < ll->length && current != NULL; j++) {
            free(current->data);
            current = current->next;
        }
        ll_free(ll);
    }
    free(ht);
}

void hash_table_print(hash_table ht) {
  if (ht == NULL) {
      return;
  }
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