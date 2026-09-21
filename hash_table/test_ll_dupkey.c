/* Regression test: re-inserting an existing key must replace its value,
 * and delete must remove the key entirely (no stale duplicates).
 *
 * Build & run: gcc -g -fsanitize=address -o t test_ll_dupkey.c && ./t
 */
#include "hash_table_ll.h"

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    int a = 1, b = 2;
    int failures = 0;

    hash_table ht = hash_table_new();
    hash_table_insert(ht, &a, 42);

    hnode * n1 = hash_table_search(ht, 42);
    printf("after 1st insert search(42) -> %d (want 1): %s\n",
           n1 ? *(int *)n1->data : -1, (n1 && *(int *)n1->data == 1) ? "PASS" : "FAIL");
    if (!n1 || *(int *)n1->data != 1) failures++;

    hash_table_insert(ht, &b, 42);   /* same key: must upsert, not append */
    hnode * n2 = hash_table_search(ht, 42);
    printf("after 2nd insert search(42) -> %d (want 2): %s\n",
           n2 ? *(int *)n2->data : -1, (n2 && *(int *)n2->data == 2) ? "PASS" : "FAIL");
    if (!n2 || *(int *)n2->data != 2) failures++;

    hash_table_delete(ht, 42);       /* must remove the key entirely */
    hnode * n3 = hash_table_search(ht, 42);
    printf("after delete search(42) -> %s (want NULL): %s\n",
           n3 ? "FOUND" : "NULL", n3 ? "FAIL" : "PASS");
    if (n3) failures++;

    hash_table_free(ht);
    printf("%s\n", failures == 0 ? "ALL PASS" : "FAILURES");
    return failures == 0 ? 0 : 1;
}
