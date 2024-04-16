#include "lp-hash.h"

int
main()
{
  hashtable_ii_entry_t* e = NULL;
  hashtable_ii_entry_t entry = { 0 };
  lphashtable_ii_t* table =
      lphashtable_ii_with_capacity(1000, 128 * 128 * 100, 0.75);
  for (int i = 0; i < 10000000; i++) {
    entry.key = i;
    entry.value = i;
    int exist = 0;
    lphashtable_ii_put(table, &entry, 0, &exist);
    assert(exist == 0);
  }
  for (int i = 0; i < 10000000; i++) {
    int exist = 0;
    e = lphashtable_ii_get(table, i, &exist);
    assert(exist == 1);
    assert(e->key == e->value);
  }
  assert(table->size == 10000000);
  lphashtable_ii_free(table);
  printf("test pass!\n");
}
