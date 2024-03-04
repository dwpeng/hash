#include "hash.h"
#include <assert.h>
#include <stdio.h>

int
main()
{
  hash_table_ii_t* table = hash_table_ii_init(10000000, 5);
  hash_table_ii_entry_t entry = { 0 };
  int N = 10000000;
  for (int i = 0; i < N; i++) {
    entry.key = i;
    entry.value = i;
    hash_table_ii_put(table, &entry);
  }
  for (int i = 0; i < table->m; i++) {
    printf("table.size[%d]: %f\n", i,
           (double)table->array[i].size / (double)table->array[i].prime);
  }
  printf("scale.size: %lu\n", table->scale_array->size);
  printf("table size: %lu\n", table->size);
  int found = 0;
  for (int i = 0; i < N; i++) {
    hash_table_ii_entry_t* e = hash_table_ii_get(table, i, &found);
    assert(i == e->key);
    assert(found);
  }
  hash_table_ii_free(table);

  hash_set_i_t* set = hash_set_i_init(10000000, 5);
  hash_set_i_entry_t set_entry = { 0 };
  for (int i = 0; i < N; i++) {
    set_entry.key = i;
    hash_set_i_put(set, &set_entry);
  }
  for (int i = 0; i < set->m; i++) {
    printf("set.size[%d]: %f\n", i,
           (double)set->array[i].size / (double)set->array[i].prime);
  }
  printf("scale.size: %lu\n", set->scale_array->size);
  printf("set size: %lu\n", set->size);
  for (int i = 0; i < N; i++) {
    int found = 0;
    hash_set_i_entry_t* e = hash_set_i_get(set, i, &found);
    assert(found);
    assert(i == e->key);
  }
  hash_set_i_free(set);
  return 0;
}
