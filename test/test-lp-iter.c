#include "lp-hash.h"

int
main()
{
  hashtable_ii_entry_t* e;
  hashtable_ii_entry_t entry = { 0 };
  int exist;
  lphashtable_ii_t* t = lphashtable_ii_with_capacity(1000, 1024 * 4, 0.75);
  for(int i = 0; i < 1000; i++) {
    entry.key = i;
    entry.value = i;
    lphashtable_ii_put(t, &entry, 0, &exist);
  }
  int count = 0;
  while((e = lphashtable_ii_iter(t)) != NULL) {
    assert(e->key == e->value);
    assert(e->key == count);
    count++;
  }
  assert(count == 1000);
  lphashtable_ii_reset_iter(t);
  count = 0;
  while((e = lphashtable_ii_iter(t)) != NULL) {
    assert(e->key == e->value);
    assert(e->key == count);
    count++;
  }
  assert(count == 1000);
  lphashtable_ii_free(t);
}
