#include "hash.h"

// pack: key: 20
// pack: value: 12

#define get_pack_hash(key) __hash_hash_u32(key)
#define pack_key_seq(a, b) __hash_eq_number(a, b)
#define get_pack_key(ek) ((ek >> 12))
define_hashset(pack, uint32_t, pack_key_seq, get_pack_hash, get_pack_key);

int
main()
{
  hashset_pack_t* set = hashset_pack_with_capacity(1000, 5, 0.75);
  hashset_pack_entry_t entry = { 0 };
  int exist = 0;
  for (int i = 0; i < 100; i++) {
    entry.key = (uint32_t)i << 12 | i;
    hashset_pack_put(set, &entry, 0, &exist);
  }
  assert(set->size == 100);
  hashset_pack_entry_t* e = NULL;
  int found = 0;
  for (int i = 0; i < 100; i++) {
    e = hashset_pack_get(set, i, &found);
    assert(e != NULL);
    assert(e->key >> 12 == i);
  }
  hashset_pack_free(set);
}
