/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "hash_table_resize.c"

hash_table * hash_table_new();
void hash_table_insert(hash_table * ht, void * d, int key);
void * hash_table_search(hash_table * ht, int key);