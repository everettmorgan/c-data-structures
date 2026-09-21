/* Regression test: open-addressing behaviors the original resize variant
 * got wrong - collision must probe (not clobber), search must match keys,
 * growth must preserve every entry, tombstones must not hide survivors.
 *
 * Build and run: gcc -g -fsanitize=address -o t test_resize.c && ./t
 */
#include "hash_table_resize.h"

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    int failures = 0;

    int v1 = 10, v3 = 30, v7 = 70, v12 = 120, vneg = -99;
    hash_table * ht = hash_table_new();

    /* keys 1 and 12 collide at capacity 11 (both % 11 == 1): probe, do not clobber */
    hash_table_insert(ht, &v1, 1);
    hash_table_insert(ht, &v12, 12);
    entry * n1 = hash_table_search(ht, 1);
    entry * n12 = hash_table_search(ht, 12);
    int ok = n1 && n12 && *(int *)n1->data == 10 && *(int *)n12->data == 120;
    printf("colliding keys 1/12 both found: %s\n", ok ? "PASS" : "FAIL");
    if (!ok) failures++;

    /* the original killer sequence: insert 1, 3, 7 destroyed key 1 data */
    hash_table_insert(ht, &v3, 3);
    hash_table_insert(ht, &v7, 7);
    n1 = hash_table_search(ht, 1);
    entry * n3 = hash_table_search(ht, 3);
    entry * n7 = hash_table_search(ht, 7);
    ok = n1 && n3 && n7 &&
         *(int *)n1->data == 10 && *(int *)n3->data == 30 && *(int *)n7->data == 70;
    printf("keys 1/3/7 all survive (old clobber case): %s\n", ok ? "PASS" : "FAIL");
    if (!ok) failures++;

    /* upsert: same key twice replaces the value */
    int v1b = 11;
    hash_table_insert(ht, &v1b, 1);
    n1 = hash_table_search(ht, 1);
    ok = n1 && *(int *)n1->data == 11;
    printf("upsert replaces value: %s\n", ok ? "PASS" : "FAIL");
    if (!ok) failures++;

    /* tombstone delete: key gone, neighbours still reachable */
    hash_table_delete(ht, 12);
    ok = hash_table_search(ht, 12) == NULL;
    printf("delete removes key: %s\n", ok ? "PASS" : "FAIL");
    if (!ok) failures++;
    n1 = hash_table_search(ht, 1);
    ok = n1 && *(int *)n1->data == 11;
    printf("probe past tombstone still finds key 1: %s\n", ok ? "PASS" : "FAIL");
    if (!ok) failures++;

    /* negative key normalizes into range */
    hash_table_insert(ht, &vneg, -1);
    entry * nn = hash_table_search(ht, -1);
    ok = nn && *(int *)nn->data == -99;
    printf("negative key round-trips: %s\n", ok ? "PASS" : "FAIL");
    if (!ok) failures++;

    /* growth: force several rehashes, every entry must survive */
    int vals[200];
    for (int i = 0; i < 200; i++) {
        vals[i] = i;
        hash_table_insert(ht, &vals[i], 1000 + i);
    }
    int all_found = 1;
    for (int i = 0; i < 200; i++) {
        entry * ne = hash_table_search(ht, 1000 + i);
        if (ne == NULL || *(int *)ne->data != i) all_found = 0;
    }
    ok = all_found && ht->capacity > DEFAULT_CAPACITY && ht->count == 204;
    printf("200 inserts across rehashes, all found (capacity %d, count %d): %s\n",
           ht->capacity, ht->count, ok ? "PASS" : "FAIL");
    if (!ok) failures++;

    hash_table_free(ht);
    printf("%s\n", failures == 0 ? "ALL PASS" : "FAILURES");
    return failures == 0 ? 0 : 1;
}
