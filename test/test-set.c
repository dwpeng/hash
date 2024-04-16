#include "hash.h"
#include <assert.h>

int
main()
{
  hashset_i_t* set = hashset_i_with_capacity(1000, 5, 0.75);
  hashset_i_entry_t entry = { 0 };
  int prev_sum = 0;
  uint64_t N = 100000;
  int exist = 0;
  for (uint64_t i = 0; i < N; i++) {
    entry.key = i;
    hashset_i_put(set, &entry, 0, &exist);
    prev_sum += i;
  }
  assert(set->size == N);
  hashset_i_entry_t* e = NULL;
  int sum = 0;
  while ((e = hashset_i_iter(set)) != NULL) {
    sum += e->key;
  }
  assert(sum == prev_sum);
  hashset_i_free(set);
  return 0;
}
