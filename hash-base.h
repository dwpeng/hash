#ifndef __hash_bash_h__
#define __hash_bash_h__

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef HASH_MALLOC
#define hash_malloc(size) malloc(size)
#define hash_free(ptr) free(ptr)
#define hash_realloc(ptr, size) realloc(ptr, size)
#endif

#define HASH_LOAD_FACTOR 0.75

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

static inline uint64_t
__lh3_Jenkins_hash_64(uint64_t key)
{
  key += ~(key << 32);
  key ^= (key >> 22);
  key += ~(key << 13);
  key ^= (key >> 8);
  key += (key << 3);
  key ^= (key >> 15);
  key += ~(key << 27);
  key ^= (key >> 31);
  return key;
}

static inline uint64_t
__hash_roundup64_dwp__(uint64_t x)
{
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x |= x >> 32;
  x++;
  return x;
}

static inline uint32_t
__string_hashcode(const char* s)
{
  uint32_t h = *s;
  if (h)
    for (++s; *s; ++s)
      h = (h << 5) - h + *s;
  return h;
}

#ifndef __is_set
#define __is_set(flags, index) (((flags)[(index) / 64] >> ((index) % 64)) & 1)
#endif
#ifndef __set
#define __set(flags, index) ((flags)[(index) / 64] |= (1LLU << ((index) % 64)))
#endif

#define __hash_eq_number(a, b) ((a) == (b))
#define __hash_eq_string(a, b) (strcmp((a), (b)) == 0)
#define __hash_hash_u32(key) __lh3_Jenkins_hash_int(key)
#define __hash_hash_u64(key) __lh3_Jenkins_hash_64(key)
#define __hash_hash_string(key) __string_hashcode(key)

#define __define_hash_table_entry(name, ktype, vtype)                         \
  typedef struct {                                                            \
    ktype key;                                                                \
    vtype value;                                                              \
  } hash##name##_entry_t;

#define __define_hash_set_entry(name, ktype)                                  \
  typedef struct {                                                            \
    ktype key;                                                                \
  } hash##name##_entry_t;

#endif // __hash_base_h__
