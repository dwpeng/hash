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
hashtable_ii_t *h = hashtable_ii_init(N, M);

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

## complex example
```c
#include "hash.h"
#include <assert.h>
#include <stdio.h>

typedef struct {
  char id[10];
  int age;
  char name;
} person_t;

#define int_eq(a, b) (b == b)
#define int_hash(a) (__lh3_Jenkins_hash_int(a))
define_hashtable(person, int, person_t*, int_eq, int_hash);

int
main()
{
  person_t persons[] = {
    { "1", 10, 'a' },
    { "2", 20, 'b' },
    { "3", 30, 'c' },
    { "4", 40, 'd' },
    { "5", 50, 'e' },
    { "6", 60, 'f' },
    { "7", 70, 'g' },
    { "8", 80, 'h' },
    { "9", 90, 'i' },
    { "10",100,'j' },
  };
  int N = 10;
  int M = 5;
  hashtable_person_t* table = hashtable_person_init(N, M);
  hashtable_person_entry_t entry = { 0 };
  for (int i = 0; i < N; i++) {
    entry.key = i;
    entry.value = &persons[i];
    hashtable_person_put(table, &entry);
  }
  for (int i = 0; i < N; i++) {
    int found = 0;
    hashtable_person_entry_t* e = hashtable_person_get(table, i, &found);
    assert(found);
    assert(persons[i].age == e->value->age);
    printf("person[%s]: %d\n", e->value->id, e->value->age);
  }
  hashtable_person_free(table);
}
```
