/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Count every allocation the hash table makes so teardown can be asserted. */
static size_t allocation_count;
static size_t outstanding_allocations;

/* Number of allocations that still succeed before failures are injected. */
#define NO_FAILURE_INJECTION (-1)

static int allocations_before_failure = NO_FAILURE_INJECTION;

static int allocation_should_fail(void) {
  if (allocations_before_failure == NO_FAILURE_INJECTION) {
    return 0;
  }
  if (allocations_before_failure == 0) {
    return 1;
  }
  allocations_before_failure--;
  return 0;
}

static void inject_failure_after(int successful_allocations) {
  assert(successful_allocations >= 0);
  allocations_before_failure = successful_allocations;
}

static void clear_failure_injection(void) {
  allocations_before_failure = NO_FAILURE_INJECTION;
}

static void *test_malloc(size_t size) {
  if (allocation_should_fail()) {
    return NULL;
  }
  void *allocation = malloc(size);
  if (allocation != NULL) {
    allocation_count++;
    outstanding_allocations++;
  }
  return allocation;
}

static void *test_calloc(size_t count, size_t size) {
  if (allocation_should_fail()) {
    return NULL;
  }
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
#include "hash_table_resize.h"
#include "hash_table_resize.h"
#undef malloc
#undef calloc
#undef free

_Static_assert(DEFAULT_SIZE > 1, "hash macro needs a divisor above one");
_Static_assert(INCREMENT > 0, "resizing must grow the table");

enum {
  FIRST_KEY = 1,
  COLLIDING_KEY = 5,
  SECOND_KEY = 2,
  SECOND_COLLIDING_KEY = 8,
  THIRD_COLLIDING_KEY = 4,
  ABSENT_KEY = 3
};

static int collides_with(int key, int other_key) {
  return hash(DEFAULT_SIZE, key) == hash(DEFAULT_SIZE, other_key);
}

static void assert_no_outstanding_allocations(void) {
  assert(outstanding_allocations == 0);
}

static void assert_empty_table(hash_table *ht) {
  assert(ht != NULL);
  for (int i = 0; i < ht->size; i++) {
    assert(ht->table[i] == NULL);
  }
}

/* A lookup returns the bucket occupant, so the key has to be checked too. */
static void assert_holds(hash_table *ht, int key, void *expected_data) {
  node *found = hash_table_search(ht, key);
  assert(found != NULL);
  assert(found->key == key);
  assert(found->data == expected_data);
}

static void assert_absent(hash_table *ht, int key) {
  node *found = hash_table_search(ht, key);
  assert(found == NULL || found->key != key);
}

static void test_free_null_table(void) {
  size_t allocations_before = allocation_count;
  size_t outstanding_before = outstanding_allocations;

  hash_table_free(NULL);

  assert(allocation_count == allocations_before);
  assert(outstanding_allocations == outstanding_before);
}

static void test_null_table_guards(void) {
  size_t allocations_before = allocation_count;
  size_t outstanding_before = outstanding_allocations;
  int value = 11;

  assert(hash_table_search(NULL, FIRST_KEY) == NULL);
  hash_table_insert(NULL, &value, FIRST_KEY);
  hash_table_print(NULL);
  assert(resize_hash_table(NULL) == NULL);

  assert(allocation_count == allocations_before);
  assert(outstanding_allocations == outstanding_before);
}

static void test_new_and_free_empty_table(void) {
  size_t allocations_before = allocation_count;

  hash_table *ht = hash_table_new();
  assert(ht != NULL);
  /* The struct and the bucket array are the only allocations. */
  assert(allocation_count == allocations_before + 2);
  assert(ht->size == DEFAULT_SIZE);
  assert_empty_table(ht);
  assert(hash_table_search(ht, FIRST_KEY) == NULL);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_insert_search_and_free(void) {
  int first = 11;
  int second = 22;

  hash_table *ht = hash_table_new();
  assert(ht != NULL);
  hash_table_insert(ht, &first, FIRST_KEY);
  hash_table_insert(ht, &second, SECOND_KEY);

  node *found = hash_table_search(ht, FIRST_KEY);
  assert(found != NULL);
  assert(found->key == FIRST_KEY);
  assert(found->data == &first);

  assert_holds(ht, SECOND_KEY, &second);

  assert_absent(ht, COLLIDING_KEY);
  assert_absent(ht, ABSENT_KEY);
  /* The table owns the wrappers only, so the values are untouched. */
  assert(first == 11);
  assert(second == 22);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_reinsert_same_key_replaces_entry(void) {
  int old_value = 11;
  int new_value = 22;

  hash_table *ht = hash_table_new();
  assert(ht != NULL);
  hash_table_insert(ht, &old_value, FIRST_KEY);
  size_t outstanding_after_first_insert = outstanding_allocations;
  assert(outstanding_after_first_insert == 3);

  hash_table_insert(ht, &new_value, FIRST_KEY);
  assert(outstanding_allocations == outstanding_after_first_insert);

  node *found = hash_table_search(ht, FIRST_KEY);
  assert(found != NULL);
  assert(found->data == &new_value);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_collision_triggers_resize(void) {
  int first = 11;
  int second = 22;
  assert(collides_with(FIRST_KEY, COLLIDING_KEY));

  hash_table *ht = hash_table_new();
  assert(ht != NULL);
  hash_table_insert(ht, &first, FIRST_KEY);
  assert(ht->size == DEFAULT_SIZE);

  hash_table_insert(ht, &second, COLLIDING_KEY);
  assert(ht->size == DEFAULT_SIZE + INCREMENT);

  assert_holds(ht, FIRST_KEY, &first);
  assert_holds(ht, COLLIDING_KEY, &second);
  /* One wrapper per entry survives the move into the larger table. */
  assert(outstanding_allocations == 4);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_recursive_resize_during_reinsertion(void) {
  int first = 11;
  int second = 22;
  int third = 33;

  hash_table *ht = hash_table_new();
  assert(ht != NULL);
  /* These two share a bucket only in the table the resize creates. */
  assert(!collides_with(SECOND_KEY, SECOND_COLLIDING_KEY));
  hash_table_insert(ht, &first, SECOND_KEY);
  hash_table_insert(ht, &second, SECOND_COLLIDING_KEY);
  assert(ht->size == DEFAULT_SIZE);

  hash_table_insert(ht, &third, THIRD_COLLIDING_KEY);
  assert(ht->size == DEFAULT_SIZE + 2 * INCREMENT);

  assert_holds(ht, SECOND_KEY, &first);
  assert_holds(ht, SECOND_COLLIDING_KEY, &second);
  assert_holds(ht, THIRD_COLLIDING_KEY, &third);
  assert(outstanding_allocations == 5);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_resize_hash_table_standalone(void) {
  hash_table *ht = hash_table_new();
  assert(ht != NULL);

  hash_table *resized = resize_hash_table(ht);
  assert(resized != NULL);
  assert(resized->size == DEFAULT_SIZE + INCREMENT);
  assert_empty_table(resized);
  hash_table_free(resized);
  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_new_malloc_failure(void) {
  size_t outstanding_before = outstanding_allocations;
  inject_failure_after(0);

  assert(hash_table_new() == NULL);

  clear_failure_injection();
  assert(outstanding_allocations == outstanding_before);
}

static void test_new_calloc_failure(void) {
  size_t outstanding_before = outstanding_allocations;
  /* The struct allocation succeeds, the bucket array does not. */
  inject_failure_after(1);

  assert(hash_table_new() == NULL);

  clear_failure_injection();
  assert(outstanding_allocations == outstanding_before);
}

static void test_node_new_malloc_failure(void) {
  int value = 11;
  size_t outstanding_before = outstanding_allocations;
  inject_failure_after(0);

  assert(hash_table_node_new(&value, FIRST_KEY) == NULL);

  clear_failure_injection();
  assert(outstanding_allocations == outstanding_before);

  node *created = hash_table_node_new(&value, FIRST_KEY);
  assert(created != NULL);
  assert(created->data == &value);
  assert(created->key == FIRST_KEY);
  test_free(created);
  assert_no_outstanding_allocations();
}

static void test_resize_hash_table_allocation_failures(void) {
  hash_table *ht = hash_table_new();
  assert(ht != NULL);
  size_t outstanding_before = outstanding_allocations;

  inject_failure_after(0);
  assert(resize_hash_table(ht) == NULL);
  clear_failure_injection();
  assert(outstanding_allocations == outstanding_before);

  inject_failure_after(1);
  assert(resize_hash_table(ht) == NULL);
  clear_failure_injection();
  assert(outstanding_allocations == outstanding_before);
  assert(ht->size == DEFAULT_SIZE);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_insert_node_allocation_failure(void) {
  int value = 11;
  hash_table *ht = hash_table_new();
  assert(ht != NULL);
  size_t outstanding_before = outstanding_allocations;

  inject_failure_after(0);
  hash_table_insert(ht, &value, FIRST_KEY);
  clear_failure_injection();

  assert(ht->size == DEFAULT_SIZE);
  assert(hash_table_search(ht, FIRST_KEY) == NULL);
  assert(outstanding_allocations == outstanding_before);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_insert_resize_malloc_failure(void) {
  int first = 11;
  int second = 22;
  hash_table *ht = hash_table_new();
  assert(ht != NULL);
  hash_table_insert(ht, &first, FIRST_KEY);
  size_t outstanding_before = outstanding_allocations;

  inject_failure_after(0);
  hash_table_insert(ht, &second, COLLIDING_KEY);
  clear_failure_injection();

  assert(ht->size == DEFAULT_SIZE);
  node *found = hash_table_search(ht, FIRST_KEY);
  assert(found != NULL);
  assert(found->data == &first);
  assert_absent(ht, COLLIDING_KEY);
  assert(outstanding_allocations == outstanding_before);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_insert_resize_calloc_failure(void) {
  int first = 11;
  int second = 22;
  hash_table *ht = hash_table_new();
  assert(ht != NULL);
  hash_table_insert(ht, &first, FIRST_KEY);
  size_t outstanding_before = outstanding_allocations;

  inject_failure_after(1);
  hash_table_insert(ht, &second, COLLIDING_KEY);
  clear_failure_injection();

  assert(ht->size == DEFAULT_SIZE);
  node *found = hash_table_search(ht, FIRST_KEY);
  assert(found != NULL);
  assert(found->data == &first);
  assert_absent(ht, COLLIDING_KEY);
  assert(outstanding_allocations == outstanding_before);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_insert_reinserted_node_allocation_failure(void) {
  int first = 11;
  int second = 22;
  hash_table *ht = hash_table_new();
  assert(ht != NULL);
  hash_table_insert(ht, &first, FIRST_KEY);
  size_t outstanding_before = outstanding_allocations;

  /* The replacement table cannot hold a wrapper for the existing entry. */
  inject_failure_after(2);
  hash_table_insert(ht, &second, COLLIDING_KEY);
  clear_failure_injection();

  /* A resize that cannot move every entry leaves the old table untouched. */
  assert(ht->size == DEFAULT_SIZE);
  assert_holds(ht, FIRST_KEY, &first);
  assert_absent(ht, COLLIDING_KEY);
  assert(outstanding_allocations == outstanding_before);

  hash_table_insert(ht, &second, COLLIDING_KEY);
  assert(ht->size == DEFAULT_SIZE + INCREMENT);
  assert_holds(ht, FIRST_KEY, &first);
  assert_holds(ht, COLLIDING_KEY, &second);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

static void test_insert_new_entry_node_allocation_failure(void) {
  int first = 11;
  int second = 22;
  hash_table *ht = hash_table_new();
  assert(ht != NULL);
  hash_table_insert(ht, &first, FIRST_KEY);
  size_t outstanding_before = outstanding_allocations;

  /* The resize succeeds, then the wrapper for the new entry cannot be made. */
  inject_failure_after(3);
  hash_table_insert(ht, &second, COLLIDING_KEY);
  clear_failure_injection();

  assert(ht->size == DEFAULT_SIZE + INCREMENT);
  assert_holds(ht, FIRST_KEY, &first);
  assert_absent(ht, COLLIDING_KEY);
  assert(outstanding_allocations == outstanding_before);

  hash_table_insert(ht, &second, COLLIDING_KEY);
  assert_holds(ht, COLLIDING_KEY, &second);

  hash_table_free(ht);
  assert_no_outstanding_allocations();
}

int main(void) {
  test_free_null_table();
  test_null_table_guards();
  test_new_and_free_empty_table();
  test_insert_search_and_free();
  test_reinsert_same_key_replaces_entry();
  test_collision_triggers_resize();
  test_recursive_resize_during_reinsertion();
  test_resize_hash_table_standalone();
  test_new_malloc_failure();
  test_new_calloc_failure();
  test_node_new_malloc_failure();
  test_resize_hash_table_allocation_failures();
  test_insert_node_allocation_failure();
  test_insert_resize_malloc_failure();
  test_insert_resize_calloc_failure();
  test_insert_reinserted_node_allocation_failure();
  test_insert_new_entry_node_allocation_failure();
  return 0;
}
