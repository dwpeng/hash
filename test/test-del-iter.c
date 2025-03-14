#include "lp-hash.h"

int
main()
{
  hashtable_ii_entry_t* e;
  hashtable_ii_entry_t entry = { 0 };
  lphashtable_ii_t* table =
      lphashtable_ii_with_capacity(100, 128 * 128 * 100, 0.75);
  for (int i = 0; i < 100; i++) {
    entry.key = i;
    entry.value = i;
    int exist = 0;
    lphashtable_ii_put(table, &entry, 0, &exist);
    assert(exist == 0);
  }
  int count = 0;
  while ((e = lphashtable_ii_iter(table)) != NULL) {
    count++;
  }
  assert(count == 100);
  lphashtable_reset_iter(ii, table);
  int ret = lphashtable_ii_del(table, 0);
  assert(ret == 1);
  e = lphashtable_ii_get(table, 0, &ret);
  assert(ret == 0);
  assert(e == NULL);
  assert(table->size == 100 - 1);
  count = 0;
  while ((e = lphashtable_ii_iter(table)) != NULL) {
    count++;
  }
  assert(count == 100 - 1);
  lphashtable_ii_free(table);
  return 0;
}
