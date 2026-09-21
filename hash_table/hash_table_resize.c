/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef HASH_TABLE_RESIZE
#define HASH_TABLE_RESIZE

#include<stdio.h>
#include<stdlib.h>

/* Open addressing hash table: linear probing for collisions, tombstones
 * for deletion, and load-factor driven growth (prime capacity, full
 * rehash). Unlike a collision-triggered resize, growth here is decided
 * by occupancy: a single collision never grows the table. */

#ifndef DEFAULT_CAPACITY
  #define DEFAULT_CAPACITY 11   // prime
#endif

typedef enum { SLOT_EMPTY, SLOT_OCCUPIED, SLOT_TOMB } slot_state;
// SLOT_EMPTY must stay 0: new tables come from calloc and are empty by zero-fill.

typedef struct hash_table_entry {
    slot_state state;
    int key;
    void * data;
} entry;

typedef struct hash_table {
    int capacity;  // prime bucket count
    int count;     // occupied slots
    int tombs;     // tombstones (load counts them; grow drops them)
    entry * table;
} hash_table;

// hash_key : positive modulo so any int key maps to [0, capacity).
static int hash_key(int key, int capacity) {
    return ((key % capacity) + capacity) % capacity;
}

static int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

static int next_prime(int n) {
    while (!is_prime(n)) n++;
    return n;
}

/* find_slot : probe sequence for key. Returns the index of the matching
 * OCCUPIED slot, or - when the key is absent - the index of the first
 * EMPTY slot (the insertion point). Tombstones are skipped so keys
 * beyond a deleted slot remain reachable. Unreachable when the load
 * factor invariant holds: an EMPTY slot always exists. */
static int find_slot(hash_table * ht, int key) {
    int i = hash_key(key, ht->capacity);
    for (int probe = 0; probe < ht->capacity; probe++) {
        entry * e = &ht->table[i];
        if (e->state == SLOT_EMPTY) return i;
        if (e->state == SLOT_OCCUPIED && e->key == key) return i;
        i = (i + 1) % ht->capacity;
    }
    return -1;
}

// hash_table_new : returns a newly allocated, empty hash table.
hash_table * hash_table_new(void) {
    hash_table * ht = malloc(sizeof(hash_table));
    ht->capacity = DEFAULT_CAPACITY;
    ht->count = 0;
    ht->tombs = 0;
    ht->table = calloc(ht->capacity, sizeof(entry));
    return ht;
}

/* grow : doubles capacity (snapped to a prime) and rehashes occupied
 * entries straight from the old array - never through insert, so no
 * nested growth. Tombstones are dropped, the old array is freed. */
static void grow(hash_table * ht) {
    int old_cap = ht->capacity;
    entry * old = ht->table;

    ht->capacity = next_prime(old_cap * 2);
    ht->table = calloc(ht->capacity, sizeof(entry));
    ht->tombs = 0;   // count carries over; tombs are gone after rehash

    for (int i = 0; i < old_cap; i++) {
        if (old[i].state == SLOT_OCCUPIED) {
            int j = hash_key(old[i].key, ht->capacity);
            while (ht->table[j].state == SLOT_OCCUPIED)
                j = (j + 1) % ht->capacity;
            ht->table[j] = old[i];
        }
    }
    free(old);
}

/* hash_table_insert : stores d under key. An existing key's value is
 * replaced in place; growth runs after a new entry lands, keyed on
 * load factor (occupied + tombs reaching 3/4 capacity). */
void hash_table_insert(hash_table * ht, void * d, int key) {
    int i = find_slot(ht, key);
    if (i < 0) return;   // unreachable under the load factor

    entry * e = &ht->table[i];
    if (e->state == SLOT_OCCUPIED) {   // upsert: same key, new value
        e->data = d;
        return;
    }

    e->state = SLOT_OCCUPIED;
    e->key = key;
    e->data = d;
    ht->count++;

    if ((ht->count + ht->tombs) * 4 >= ht->capacity * 3)
        grow(ht);
}

// hash_table_search : returns the entry for key, or NULL when absent.
entry * hash_table_search(hash_table * ht, int key) {
    int i = find_slot(ht, key);
    if (i < 0) return NULL;
    entry * e = &ht->table[i];
    if (e->state == SLOT_OCCUPIED && e->key == key) return e;
    return NULL;   // hit an EMPTY slot: key was never inserted
}

/* hash_table_delete : marks key's slot as a tombstone. The probe path
 * stays intact so entries stored past the slot remain findable; the
 * tombstone is reclaimed by the next grow. */
void hash_table_delete(hash_table * ht, int key) {
    int i = find_slot(ht, key);
    if (i < 0) return;
    entry * e = &ht->table[i];
    if (e->state == SLOT_OCCUPIED && e->key == key) {
        e->state = SLOT_TOMB;
        e->data = NULL;
        ht->count--;
        ht->tombs++;
    }
}

// hash_table_free : frees the table. Entries are inline; caller-owned data untouched.
void hash_table_free(hash_table * ht) {
    free(ht->table);
    free(ht);
}

void hash_table_print(hash_table * ht) {
    printf("***---***\n");
    printf("capacity: %d  count: %d  tombs: %d\n", ht->capacity, ht->count, ht->tombs);
    for (int i = 0; i < ht->capacity; i++) {
        entry * e = &ht->table[i];
        const char * mark = e->state == SLOT_OCCUPIED ? "     "
                          : e->state == SLOT_TOMB    ? "tomb "
                          :                            "empty";
        printf("slot %2d: %s key=%d\n", i, mark,
               e->state == SLOT_OCCUPIED ? e->key : -1);
    }
}

#endif // HASH_TABLE_RESIZE
