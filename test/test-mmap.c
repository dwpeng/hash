
#define HASH_MMAP
#include "hash.h"

int
main()
{
  hashset_i_t* set = hashset_i_init(10000, 5);
  hashset_i_entry_t entry = { 0 };
  int prev_sum = 0;
  for (int i = 0; i < 10000; i++) {
    entry.key = i;
    hashset_i_put(set, &entry);
    prev_sum += i;
  }
  assert(set->size == 10000);
  hashset_i_entry_t* e = NULL;
  int sum = 0;
  while ((e = hashset_i_iter(set)) != NULL) {
    sum += e->key;
  }
  assert(sum == prev_sum);
  hashset_i_free(set);
  return 0;
}
