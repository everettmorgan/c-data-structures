/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include<stdio.h>
#include<stdlib.h>
#include "hash_table_resize.h"

int main() {
    char a = 'h';
    char b = 'e';
    char c = 'l';
    char d = 'l';
    char e = 'o';

    hash_table * ht = hash_table_new();

    hash_table_insert(ht, &b, (int)a+15);
    hash_table_insert(ht, &c, (int)b+15);
    hash_table_insert(ht, &d, (int)c+15);
    hash_table_insert(ht, &e, (int)d+15);
    hash_table_insert(ht, &e, (int)e+15);

    hash_table_print(ht);

    node * n = hash_table_search(ht, 11);
    if (n != NULL) {
        printf("found %c at %p\n", *(char *)n->data, n);
    }

    return 0;
}