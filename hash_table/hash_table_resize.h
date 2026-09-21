/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "./hash_table_resize.c"

hash_table * hash_table_new(void);
void hash_table_insert(hash_table * ht, void * d, int key);
entry * hash_table_search(hash_table * ht, int key);
void hash_table_delete(hash_table * ht, int key);
void hash_table_free(hash_table * ht);
void hash_table_print(hash_table * ht);
