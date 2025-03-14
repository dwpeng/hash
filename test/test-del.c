#include "hash.h"
#include "lp-hash.h"

void
test_lphash()
{
  hashtable_ii_entry_t* e;
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
  int ret = lphashtable_ii_del(table, 0);
  assert(ret == 1);
  e = lphashtable_ii_get(table, 0, &ret);
  assert(ret == 0);
  assert(e == NULL);
  assert(table->size == 10000000 - 1);
  lphashtable_ii_free(table);
}

void
test_hash()
{
  hashtable_ii_entry_t* e;
  hashtable_ii_entry_t entry = { 0 };
  hashtable_ii_t* table =
      hashtable_ii_with_capacity(1000, 128 * 128 * 100, 0.75);
  for (int i = 0; i < 10000000; i++) {
    entry.key = i;
    entry.value = i;
    int exist = 0;
    hashtable_ii_put(table, &entry, 0, &exist);
    assert(exist == 0);
  }
  int ret = hashtable_ii_del(table, 0);
  assert(ret == 1);
  e = hashtable_ii_get(table, 0, &ret);
  assert(ret == 0);
  assert(e == NULL);
  assert(table->size == 10000000 - 1);
  hashtable_ii_free(table);
}

int
main()
{
  test_lphash();
  test_hash();
  printf("test pass!\n");
}
