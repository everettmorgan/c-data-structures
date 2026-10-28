/* Regression test: negative keys must hash into [0, HASH_TABLE_SIZE).
 * Before the fix, key -1 produced bucket -1 (heap-buffer-overflow under ASan).
 *
 * Build & run: gcc -g -fsanitize=address -o t test_ll_negkey.c && ./t
 */
#include "hash_table_ll.h"

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    int v = -1;

    hash_table ht = hash_table_new();
    hash_table_insert(ht, &v, -1);

    hnode * n = hash_table_search(ht, -1);
    int ok = (n != NULL && *(int *)n->data == -1);
    printf("negative key insert/search: %s\n", ok ? "PASS" : "FAIL");

    hash_table_free(ht);
    return ok ? 0 : 1;
}
