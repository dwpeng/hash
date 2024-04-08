#include "hash.h"
#include <assert.h>

int
main()
{
  hashset_i_t* set = hashset_i_init(1000, 5);
  hashset_i_entry_t entry = { 0 };
  int prev_sum = 0;
  uint64_t N = 100000;
  for (uint64_t i = 0; i < N; i++) {
    entry.key = i;
    hashset_i_put(set, &entry);
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
