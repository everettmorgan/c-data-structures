/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "./hash_table_ll.h"

int main() {
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;

    hash_table ht = hash_table_new();
    hash_table_insert(ht, &a, 100);
    hash_table_insert(ht, &b, 23);
    hash_table_insert(ht, &c, 5435);
    hash_table_insert(ht, &d, 512);
    hash_table_print(ht);

    hnode * h = hash_table_search(ht, 512);
    if (h != NULL) {
        printf("found %d at %p\n", *(int *)h->data, h->data);
    }

    hash_table_delete(ht, 512);

    hnode * dh = hash_table_search(ht, 23);
    if (dh != NULL) {
        printf("found %d at %p\n", *(int *)dh->data, dh->data);
    }

    hash_table_print(ht);

    return 0;
}