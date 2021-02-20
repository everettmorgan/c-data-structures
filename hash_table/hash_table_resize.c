/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include<stdlib.h> // calloc(), exit(), realloc()

#define DEFAULT_SIZE 10
#define INCREMENT 2

typedef struct hash_table {
    void * table[DEFAULT_SIZE];
    int size;
} hash_table;

int hash(int len, int key) {
    return key % (len-1);
}

hash_table * hash_table_new() {
    hash_table * ht = calloc(DEFAULT_SIZE, sizeof(void *));
    if (ht == NULL) {
        exit(1);
    }
    ht->size = DEFAULT_SIZE;
    return ht;
}

void * hash_table_search(hash_table * ht, int key) {
    int i = hash(ht->size, key);
    if (ht->table[i] != NULL) {
        return ht->table[i];
    }
    return NULL;
}

void * crealloc(void * v, int vsz, int newsz) {
    void * n = calloc(newsz, sizeof(void *));
    for (int i = 0; i < vsz; i++) {
        n = v;
        n++;
        v++;
    }
    return n;
}

hash_table * hash_table_resize(hash_table * ht, int newsz) {
    void * error = crealloc(ht->table, ht->size, newsz);
    if (error == NULL) {
        exit(1);
    }
    ht->size = newsz;
    return ht;
}

void hash_table_insert(hash_table * ht, void * d, int key) {
    int i;
    try_to_insert: do {
        i = hash(ht->size, key);
        if (ht->table[i] != NULL) {
            hash_table_resize(ht, ht->size+INCREMENT);
            goto try_to_insert;
        }
    } while(0);
    ht->table[i] = d;
}

void hash_table_print(hash_table * ht) {
    printf("***---***\n");
    for (int i = 0; i < ht->size; i++) {
        printf("void * %p\n", ht->table[i]);
    }
    printf("***---***\n");
}