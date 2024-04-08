
#define HASH_MMAP
#include "hash.h"

define_hashset(int, int, ii_eq, ii_hash);

int
main()
{
  hashset_int_t* set = hashset_int_init(10000, 5);
  hashset_int_entry_t entry = { 0 };
  int prev_sum = 0;
  uint64_t N = 200000;
  for (uint64_t i = 0; i < N; i++) {
    entry.key = i;
    hashset_int_put(set, &entry);
    prev_sum += i;
  }
  assert(set->size == N);
  hashset_int_entry_t* e = NULL;
  int sum = 0;
  while ((e = hashset_int_iter(set)) != NULL) {
    sum += e->key;
  }
  assert(sum == prev_sum);
  hashset_int_free(set);
  return 0;
}
