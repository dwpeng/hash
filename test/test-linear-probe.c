#include "hash.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

int
main()
{
  hashtable_ii_t* table = hashtable_ii_with_capacity(100, 1, 0.8);
  hashtable_ii_entry_t entry = { 0 };
  int N = 2000000;
  int replaced = 0;
  int exists = 0;
  for (int i = 0; i < N; i++) {
    entry.key = i;
    entry.value = i;
    hashtable_ii_put(table, &entry, replaced, &exists);
  }
  assert(table->size == (uint64_t)N);
  int found = 0;
  for (int i = 0; i < N; i++) {
    hashtable_ii_entry_t* e = hashtable_ii_get(table, i, &found);
    assert(found);
    assert(i == e->value && i == e->key);
  }
  printf("hashtable size: %lu\n", table->size);
  printf("linear-probe-arr size: %lu\n", table->linear->size);
  printf("linear-probe-arr used rate: %.2f\n",
         (double)table->linear->size
             / (double)table->linear->raw.capacity);
  hashtable_ii_free(table);
}
