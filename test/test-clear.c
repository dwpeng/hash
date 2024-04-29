#include "hash.h"

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
  hashtable_ii_clear(table);
  assert(table->size == 0);
  int n = 0;
  while (hashtable_ii_iter(table)) {
    n++;
  }
  assert(n == 0);
  hashtable_ii_free(table);
  printf("PASS\n");
}
