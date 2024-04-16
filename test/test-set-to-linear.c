#include "hash.h"

int
main()
{
  printf("Test set to linear probing\n");
  int N = 10000000;
  hashtable_ii_t* dict = hashtable_ii_with_capacity(10000, 5, 0.75);
  hashtable_ii_entry_t entry = { 0 };
  hashtable_ii_entry_t* e = NULL;
  hashtable_ii_entry_t** records =
      (hashtable_ii_entry_t**)malloc(N * sizeof(hashtable_ii_entry_t*));
  int exist = 0;
  for (int i = 0; i < N; i++) {
    entry.key = i;
    entry.value = i;
    records[i] = hashtable_ii_put(dict, &entry, 0, &exist);
  }
  assert(dict->size == (uint64_t)N);

  int count = 0;
  for (int i = 0; i < N; i++) {
    e = records[i];
    assert(e->key == i);
    if (i % 1000000 == 0) {
      printf("key: %d, value: %d\n", e->key, e->value);
    }
    count++;
  }
  assert(count == N);
  hashtable_ii_free(dict);
  free(records);
  printf("Test set to linear probing passed\n");
}
