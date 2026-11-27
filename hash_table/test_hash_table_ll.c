#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Count every allocation the hash table makes so teardown can be asserted. */
static size_t allocation_count;
static size_t outstanding_allocations;

static void *test_malloc(size_t size) {
  void *allocation = malloc(size);
  if (allocation != NULL) {
    allocation_count++;
    outstanding_allocations++;
  }
  return allocation;
}

static void *test_calloc(size_t count, size_t size) {
  void *allocation = calloc(count, size);
  if (allocation != NULL) {
    allocation_count++;
    outstanding_allocations++;
  }
  return allocation;
}

static void test_free(void *allocation) {
  if (allocation == NULL) {
    return;
  }
  assert(outstanding_allocations > 0);
  outstanding_allocations--;
  free(allocation);
}

#define malloc test_malloc
#define calloc test_calloc
#define free test_free
#include "hash_table_ll.h"
#include "hash_table_ll.h"
#undef malloc
#undef calloc
#undef free

_Static_assert(HASH_TABLE_SIZE > 1, "hash macro needs a divisor above one");

enum {
  FIRST_KEY = 1,
  SECOND_KEY = 2,
  ABSENT_KEY = 3
};

static int colliding_key(int key) {
  return key + (HASH_TABLE_SIZE - 1);
}

static void assert_no_outstanding_allocations(void) {
  assert(outstanding_allocations == 0);
}

static void test_free_null_table(void) {
  size_t allocations_before = allocation_count;
  size_t outstanding_before = outstanding_allocations;

  hash_table_free(NULL);

  assert(allocation_count == allocations_before);
  assert(outstanding_allocations == outstanding_before);
}

static void test_new_and_free_empty_table(void) {
  size_t allocations_before = allocation_count;

  hash_table ht = hash_table_new();
  assert(ht != NULL);
  assert(allocation_count == allocations_before + 1);
  assert(outstanding_allocations == 1);

  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    assert(ht[i] == NULL);
  }
  assert(hash_table_search(ht, FIRST_KEY) == NULL);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_insert_search_and_free(void) {
  int first = 11;
  int second = 22;

  hash_table ht = hash_table_new();
  assert(ht != NULL);
  hash_table_insert(ht, &first, FIRST_KEY);
  hash_table_insert(ht, &second, SECOND_KEY);

  hnode *found = hash_table_search(ht, FIRST_KEY);
  assert(found != NULL);
  assert(found->key == FIRST_KEY);
  assert(found->data == &first);

  found = hash_table_search(ht, SECOND_KEY);
  assert(found != NULL);
  assert(found->key == SECOND_KEY);
  assert(found->data == &second);

  assert(hash_table_search(ht, ABSENT_KEY) == NULL);

  hash_table_free(ht);
  assert(first == 11);
  assert(second == 22);
  assert_no_outstanding_allocations();
}

static void test_colliding_keys(void) {
  int first = 11;
  int second = 22;
  int first_key = FIRST_KEY;
  int second_key = colliding_key(FIRST_KEY);
  assert(hash(first_key) == hash(second_key));

  hash_table ht = hash_table_new();
  assert(ht != NULL);
  hash_table_insert(ht, &first, first_key);
  hash_table_insert(ht, &second, second_key);

  linked_list *bucket = (linked_list *)ht[hash(first_key)];
  assert(bucket != NULL);
  assert(ll_prop(bucket, length) == 2);

  hnode *found = hash_table_search(ht, first_key);
  assert(found != NULL);
  assert(found->data == &first);
  found = hash_table_search(ht, second_key);
  assert(found != NULL);
  assert(found->data == &second);
  assert(hash_table_search(ht, colliding_key(second_key)) == NULL);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_delete_existing_key(void) {
  int kept = 22;
  int removed = 11;
  int kept_key = colliding_key(FIRST_KEY);
  int removed_key = FIRST_KEY;
  assert(hash(kept_key) == hash(removed_key));

  hash_table ht = hash_table_new();
  assert(ht != NULL);
  hash_table_insert(ht, &removed, removed_key);
  hash_table_insert(ht, &kept, kept_key);
  size_t outstanding_before_delete = outstanding_allocations;

  hash_table_delete(ht, removed_key);
  /* The list node and the hnode wrapper both belong to the table. */
  assert(outstanding_allocations == outstanding_before_delete - 2);
  assert(hash_table_search(ht, removed_key) == NULL);

  hnode *surviving = hash_table_search(ht, kept_key);
  assert(surviving != NULL);
  assert(surviving->data == &kept);
  assert(removed == 11);

  outstanding_before_delete = outstanding_allocations;
  hash_table_delete(ht, removed_key);
  assert(outstanding_allocations == outstanding_before_delete);

  hash_table_delete(ht, kept_key);
  assert(outstanding_allocations == outstanding_before_delete - 2);
  assert(hash_table_search(ht, kept_key) == NULL);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_delete_missing_key(void) {
  int value = 11;
  int present_key = FIRST_KEY;
  int missing_in_used_bucket = colliding_key(FIRST_KEY);
  int missing_in_empty_bucket = ABSENT_KEY;
  assert(hash(missing_in_used_bucket) == hash(present_key));
  assert(hash(missing_in_empty_bucket) != hash(present_key));

  hash_table ht = hash_table_new();
  assert(ht != NULL);
  hash_table_insert(ht, &value, present_key);
  size_t outstanding_before_delete = outstanding_allocations;

  hash_table_delete(ht, missing_in_used_bucket);
  assert(outstanding_allocations == outstanding_before_delete);
  hash_table_delete(ht, missing_in_empty_bucket);
  assert(outstanding_allocations == outstanding_before_delete);

  hnode *found = hash_table_search(ht, present_key);
  assert(found != NULL);
  assert(found->data == &value);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_delete_from_null_table(void) {
  size_t allocations_before = allocation_count;
  size_t outstanding_before = outstanding_allocations;

  hash_table_delete(NULL, FIRST_KEY);
  hash_table_delete(NULL, ABSENT_KEY);

  assert(allocation_count == allocations_before);
  assert(outstanding_allocations == outstanding_before);
}

int main(void) {
  test_free_null_table();
  test_new_and_free_empty_table();
  test_insert_search_and_free();
  test_colliding_keys();
  test_delete_existing_key();
  test_delete_missing_key();
  test_delete_from_null_table();
  return 0;
}
