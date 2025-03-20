## hashtable/hashset
A hashtable/hashset implementation in C. It's a generic implementation, you can define your own key and value type. 
Both multiple layers of hash table and linear probing is used to reduce the collision.
```txt
  +-----------------+ ---------------
  |  N0             |               |
  +--------------+--+               |
  |  N1          |                  |
  +----------+---+               multiple layers
  |  N2      |                      |
  +------+---+                      |
  |  N3  |                          |
  +------+---------------------+ ----
  |                            |  linear probing
  +----------------------------+ ----
```

## usage
### define hashtable/hashset
```c
// Step1: define hash_equal and hash_hash macro
static inline uint32_t
__lh3_Jenkins_hash_int(uint32_t key)
{
  key += (key << 12);
  key ^= (key >> 22);
  key += (key << 4);
  key ^= (key >> 9);
  key += (key << 10);
  key ^= (key >> 2);
  key += (key << 7);
  key ^= (key >> 12);
  return key;
}
#define hash_equal(a, b) (a == b)
#define hash_hash(key) __lh3_Jenkins_hash_int(key)
#define hash_entrykey(ekey) (ekey) // compute the key of the entry

// Step2: define hashtable
define_hashtable(ii, int, int, hash_equal, hash_hash, hash_entrykey)
```

### methods of hashtable

```c
// init
int N = 1000000; // size
int M = 5;       // layers
hashtable_ii_t *h = hashtable_ii_init(N, M);

int replace = false; // don't replace the value if the key exists
int exist = 0;   // check if the key exists
// put
hashtable_ii_entry_t entry = {
  .key = 1,
  .value = 1
};
hashtable_ii_entry_t* e = NULL;
e = hashtable_ii_put(h, &entry, replace, &exist);
assert(e->key == 1 && e->value == 1 && exist == 0);
// get 
hashtable_ii_entry_t *entry = hashtable_ii_get(h, 1, &exist);
assert(entry->key == 1 && entry->value == 1 && exist == 1);
// iter
hashtable_ii_entry_t *el = NULL;
while((el = hashtable_ii_iter(h, el))) {
  printf("%d\n", el->key);
}

// free
hashtable_ii_free(h);
```

### methods of hashset
replace hashtable_ii with hashset_ii in the above example

## example
```bash
git clone git@github.com:dwpeng/hash.git
cd hash
python complie.py
```
