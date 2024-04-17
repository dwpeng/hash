#include "hash.h"

define_hash(test, int, int, __hash_eq_number, __hash_hash_u32);

int
main()
{
  hashtable_test_t* table = hashtable_with_capacity(test, 100, 1, 0.8);
  hashtable_test_entry_t entry = { 0 };
  hashtable_test_entry_t* e;
  int N = 2000000;
  int replaced = 0;
  int exists = 0;
  for (int i = 0; i < N; i++) {
    entry.key = i;
    entry.value = i;
    hashtable_put(test, table, &entry, replaced, &exists);
  }
  assert(table->size == (uint64_t)N);
  int count = 0;
  while (1) {
    e = hashtable_iter(test, table);
    if (e == NULL) {
      break;
    }
    assert(e->key == e->value);
    count++;
  }
  assert(count == N);
  for (int i = 0; i < N; i++) {
    e = hashtable_get(test, table, i, &exists);
    assert(exists);
    assert(e->key == i);
  }
  hashtable_free(test, table);

  lphashtable_test_t* ltable = lphashtable_with_capacity(test, 100, 1, 0.8);
  for (int i = 0; i < N; i++) {
    entry.key = i;
    entry.value = i;
    lphashtable_put(test, ltable, &entry, replaced, &exists);
  }
  assert(ltable->size == (uint64_t)N);
  count = 0;
  while (1) {
    e = lphashtable_iter(test, ltable);
    if (e == NULL) {
      break;
    }
    assert(e->key == e->value);
    count++;
  }
  assert(count == N);
  for (int i = 0; i < N; i++) {
    e = lphashtable_get(test, ltable, i, &exists);
    assert(exists);
    assert(e->key == i);
  }
  lphashtable_free(test, ltable);
  printf("OK\n");
}
