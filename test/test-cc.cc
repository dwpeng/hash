#include "hash.h"
#include <cassert>
#include <cstdio>

namespace hashtest {
int
test()
{
  int N = 100000;
  int M = 5;
  hashtable_ii_t* table = hashtable_ii_with_capacity(N, M, 0.8);
  hashtable_ii_entry_t entry = { 0, 0 };
  int replace = 0;
  int exist = 0;
  for (int i = 0; i < N; i++) {
    entry.key = i;
    entry.value = i;
    hashtable_ii_put(table, &entry, replace, &exist);
  }
  hashtable_ii_entry_t* e = NULL;
  int found = 0;
  for (int i = 0; i < N; i++) {
    e = hashtable_ii_get(table, i, &found);
    assert(e->key == i);
    assert(e->value == i);
    assert(found);
  }
  hashtable_ii_free(table);
  return 0;
}
} // namespace hashtest

int
main()
{
  hashtest::test();
  return 0;
}
