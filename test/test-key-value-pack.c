#include "hash.h"

// pack: key: 20
// pack: value: 12

#define hash_code(key) (__hash_hash_u32(key))
#define hash_eq(a, b) ((a) == (b))
#define hash_eq2(ek) ((ek >> 12))
define_hashset(pack, uint32_t, hash_eq, hash_code, hash_eq2);

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
