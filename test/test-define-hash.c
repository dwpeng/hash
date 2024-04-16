#include "hash.h"

define_hash(kmerfreq, uint64_t, int, __hash_eq_number, __hash_hash_u64);

int
main()
{
  hashtable_kmerfreq_t* ht = hashtable_kmerfreq_with_capacity(1000, 4, 0.75);
  hashtable_kmerfreq_entry_t entry = { 0 };
  int exists = 0;
  for (uint64_t kmer = 0; kmer < 10; kmer++) {
    entry.key = kmer;
    entry.value = kmer;
    hashtable_kmerfreq_put(ht, &entry, 0, &exists);
  }
  assert(ht->size == 10);
  hashtable_kmerfreq_free(ht);
}
