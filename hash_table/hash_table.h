#include "./hash_table.c"

void hash_table_insert(hash_table * ht, void * d, int key);
hnode * hash_table_search(hash_table * ht, int key);
void hash_table_delete(hash_table * ht, int key);