#include "hash.h"
#include <assert.h>
#include <stdio.h>

int
main()
{
  int N = 10000000;
  int M = 5;
  hashtable_ii_t* table = hashtable_ii_with_capacity(N, M, 0.5);
  hashtable_ii_entry_t entry = { 0 };
  int replaced = 0;
  int exists = 0;
  for (int i = 0; i < N; i++) {
    entry.key = i;
    entry.value = i;
    hashtable_ii_put(table, &entry, replaced, &exists);
  }
  for (int i = 0; i < table->m; i++) {
    printf("table.size[%d]: %f\n", i,
           (double)table->array[i].size / (double)table->array[i].prime);
  }
  printf("scale.size: %lu\n", table->linear->size);
  printf("table size: %lu\n", table->size);
  int found = 0;
  for (int i = 0; i < N; i++) {
    hashtable_ii_entry_t* e = hashtable_ii_get(table, i, &found);
    assert(i == e->key);
    assert(found);
  }
  hashtable_ii_entry_t* e = NULL;
  int size = 0;
  while ((e = hashtable_ii_iter(table)) != NULL) {
    size++;
    assert(e->key == e->value);
  }
  printf("size: %d\n", size);
  hashtable_ii_free(table);

  hashset_i_t* set = hashset_i_with_capacity(N, M, 0.5);
  hashset_i_entry_t set_entry = { 0 };
  for (int i = 0; i < N; i++) {
    set_entry.key = i;
    hashset_i_put(set, &set_entry, 0, &found);
  }
  for (int i = 0; i < set->m; i++) {
    printf("set.size[%d]: %f\n", i,
           (double)set->array[i].size / (double)set->array[i].prime);
  }
  printf("scale.size: %lu\n", set->linear->size);
  printf("set size: %lu\n", set->size);
  for (int i = 0; i < N; i++) {
    int found = 0;
    hashset_i_entry_t* e = hashset_i_get(set, i, &found);
    assert(found);
    assert(i == e->key);
  }
  hashset_i_free(set);
  return 0;
}
