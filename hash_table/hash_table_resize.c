/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include<stdio.h> // printf()
#include<stdlib.h> // malloc(), calloc(), exit()

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
    ht->table = calloc(DEFAULT_SIZE, sizeof(void *));
    ht->size = DEFAULT_SIZE;
    return ht;
}

node * hash_table_search(hash_table * ht, int key) {
    return retrieve(key);
}

hash_table * resize_hash_table(hash_table * ht) {
    hash_table * nht = malloc(sizeof(hash_table));
    nht->table = calloc(ht->size+INCREMENT, sizeof(void *));
    nht->size = ht->size+INCREMENT;
    return nht;
}

#define NODE_DATA (((node *)ht->table[i])->data)
#define NODE_KEY (((node *)ht->table[i])->key)

void hash_table_print(hash_table * ht) {
    printf("***---***\n");
    printf("length: %d\n", ht->size);
    for (int i = 0; i < ht->size; i++) {
        printf("addr: %p %d\n", ht->table[i], ht->table[i] ? NODE_KEY : -1);
    }
    printf("***---***\n");
}

void hash_table_insert(hash_table * ht, void * d, int key) {
    if (retrieve(key)) {
        hash_table * nht = resize_hash_table(ht);
        for (int i = 0; i < ht->size; i++) {
            if (ht->table[i]) {
                hash_table_insert(nht, NODE_DATA, NODE_KEY);
            }
        }
        *ht = *nht;
    }
    ht->table[hash(ht->size, key)] = hash_table_node_new(d, key);
}