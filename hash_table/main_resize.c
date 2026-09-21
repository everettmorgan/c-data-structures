/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "hash_table_resize.h"

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    char a = 'h';
    char b = 'e';
    char c = 'l';
    char d = 'l';   /* duplicate key of c: must upsert, not collide */
    char e = 'o';

    hash_table * ht = hash_table_new();

    hash_table_insert(ht, &a, (int)a+15);
    hash_table_insert(ht, &b, (int)b+15);
    hash_table_insert(ht, &c, (int)c+15);
    hash_table_insert(ht, &d, (int)d+15);
    hash_table_insert(ht, &e, (int)e+15);
    hash_table_print(ht);

    entry * n = hash_table_search(ht, (int)b+15);
    if (n != NULL) {
        printf("found %c at %p\n", *(char *)n->data, (void *)n);
    }

    hash_table_delete(ht, (int)a+15);
    n = hash_table_search(ht, (int)a+15);
    printf("after delete, search(%d) -> %s\n", (int)a+15, n ? "FOUND" : "NULL");
    n = hash_table_search(ht, (int)b+15);
    printf("after delete, search(%d) -> %s\n", (int)b+15, n ? "FOUND" : "NULL");

    hash_table_free(ht);
    return 0;
}
