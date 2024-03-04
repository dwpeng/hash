## hashtable/hashset

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
#define hash_equal(entry, b) ((a).key == (b))
#define hash_hash(key) __lh3_Jenkins_hash_int(key)

// Step2: define hashtable
define_hashtable(ii, int, int, hash_equal, hash_hash)
// Step3: define hashset
define_hashset(ii, int, hash_equal, hash_hash)
```

### methods of hashtable

```c
// init
int N = 1000000; // size
int M = 5;       // layers
hashtable_ii_t *h = hashtable_ii_init(M, N);

// put
hashtable_ii_entry_t entry = {
  .key = 1,
  .value = 1
};
hashtable_ii_put(h, &entry);

// get 
hashtable_ii_entry_t *entry = hashtable_ii_get(h, 1);

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
make
./test
```
