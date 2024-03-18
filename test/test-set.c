#include "hash.h"
#include <assert.h>

int
main()
{
  hashset_i_t* set = hashset_i_init(1, 5);
  hashset_i_entry_t entry = { 0 };
  int prev_sum = 0;
  for (int i = 0; i < 100; i++) {
    entry.key = i;
    hashset_i_put(set, &entry);
    prev_sum += i;
  }
  assert(set->size == 100);
  hashset_i_entry_t* e = NULL;
  int sum = 0;
  while ((e = hashset_i_iter(set)) != NULL) {
    sum += e->key;
  }
  assert(sum == prev_sum);
  return 0;
}
