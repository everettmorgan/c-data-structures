/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include<stdio.h>
#include "hash_table_resize.h"

int main() {
    char a = 'h';
    char b = 'e';
    char c = 'l';
    char d = 'l';
    char e = 'o';

    hash_table * ht = hash_table_new();

    hash_table_insert(ht, &a, 101);
    hash_table_insert(ht, &b, 51);
    hash_table_insert(ht, &c, 21);
    hash_table_insert(ht, &d, 11);
    hash_table_insert(ht, &e, 91);
    hash_table_insert(ht, &a, 141);
    hash_table_insert(ht, &b, 541);
    hash_table_insert(ht, &c, 221);
    hash_table_insert(ht, &d, 111);
    hash_table_insert(ht, &e, 981);

    hash_table_print(ht);

    void * n = hash_table_search(ht, 15);
    if (n != NULL) {
        printf("found %c at %p\n", *(char *)n, n);
    }

    hash_table_print(ht);

    return 0;
}