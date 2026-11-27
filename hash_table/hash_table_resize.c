/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef HASH_TABLE_RESIZE
#define HASH_TABLE_RESIZE

#include<stdio.h> // printf()
#include<stdlib.h> // malloc(), calloc(), free()

#define DEFAULT_SIZE 5
#define INCREMENT 2

#define hash(__len, __key) ( ( (int)__key ) % ( ( (int)__len) - 1 ) )
#define retrieve(__key) ( ht->table[ hash( ht->size, ( ( (int)__key ) ) ) ])

typedef struct hash_table_node {
    void * data;
    int key;
} node;

node * hash_table_node_new(void * d, int key) {
    node * n = malloc(sizeof(node));
    if (n == NULL) {
        return NULL;
    }
    n->data = d;
    n->key = key;
    return n;
}

typedef struct hash_table {
    int size;
    void ** table;
} hash_table;

hash_table * hash_table_new() {
    hash_table * ht = malloc(sizeof(hash_table));
    if (ht == NULL) {
        return NULL;
    }
    ht->table = calloc(DEFAULT_SIZE, sizeof(void *));
    if (ht->table == NULL) {
        free(ht);
        return NULL;
    }
    ht->size = DEFAULT_SIZE;
    return ht;
}

node * hash_table_search(hash_table * ht, int key) {
    if (ht == NULL || ht->table == NULL) {
        return NULL;
    }
    return retrieve(key);
}

hash_table * resize_hash_table(hash_table * ht) {
    if (ht == NULL) {
        return NULL;
    }
    hash_table * nht = malloc(sizeof(hash_table));
    if (nht == NULL) {
        return NULL;
    }
    nht->table = calloc(ht->size+INCREMENT, sizeof(void *));
    if (nht->table == NULL) {
        free(nht);
        return NULL;
    }
    nht->size = ht->size+INCREMENT;
    return nht;
}

#define NODE_DATA (((node *)ht->table[i])->data)
#define NODE_KEY (((node *)ht->table[i])->key)

void hash_table_print(hash_table * ht) {
    if (ht == NULL || ht->table == NULL) {
        return;
    }
    printf("***---***\n");
    printf("length: %d\n", ht->size);
    for (int i = 0; i < ht->size; i++) {
        printf("addr: %p %d\n", ht->table[i], ht->table[i] ? NODE_KEY : -1);
    }
    printf("***---***\n");
}

/* Releases the node wrappers and the bucket array, never the data they point at. */
static void hash_table_release_entries(hash_table * ht) {
    for (int i = 0; i < ht->size; i++) {
        free(ht->table[i]);
    }
    free(ht->table);
    ht->table = NULL;
}

void hash_table_free(hash_table * ht) {
    if (ht == NULL) {
        return;
    }
    if (ht->table != NULL) {
        hash_table_release_entries(ht);
    }
    free(ht);
}

/* Returns 1 when the entry was stored, 0 when it had to be dropped. */
static int hash_table_store(hash_table * ht, void * d, int key);
/* Returns 1 when the table grew, 0 when it was left exactly as it was. */
static int hash_table_rebuild(hash_table * ht);

static int hash_table_rebuild(hash_table * ht) {
    hash_table * nht = resize_hash_table(ht);
    if (nht == NULL) {
        return 0;
    }
    for (int i = 0; i < ht->size; i++) {
        if (ht->table[i]) {
            /* A re-insert may grow nht again; it updates nht in place. */
            if (!hash_table_store(nht, NODE_DATA, NODE_KEY)) {
                /* Nothing has been released yet, so the old table still owns it all. */
                hash_table_free(nht);
                return 0;
            }
        }
    }
    hash_table_release_entries(ht);
    *ht = *nht;
    free(nht);
    return 1;
}

static int hash_table_store(hash_table * ht, void * d, int key) {
    if (ht == NULL || ht->table == NULL) {
        return 0;
    }
    if (retrieve(key) && !hash_table_rebuild(ht)) {
        return 0;
    }
    node * n = hash_table_node_new(d, key);
    if (n == NULL) {
        return 0;
    }
    /* An entry with the same key is replaced, so its wrapper is released. */
    free(ht->table[hash(ht->size, key)]);
    ht->table[hash(ht->size, key)] = n;
    return 1;
}

void hash_table_insert(hash_table * ht, void * d, int key) {
    (void)hash_table_store(ht, d, key);
}

#endif
