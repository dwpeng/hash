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

/*
 *
 * The following code is from the fastmod library, which is licensed under the
 * Apache License 2.0.
 **/

// This is for the 64-bit functions.
static inline uint64_t
__dwp_mul128_u64(__uint128_t lowbits, uint64_t d)
{
  __uint128_t bottom_half =
      (lowbits & UINT64_C(0xFFFFFFFFFFFFFFFF)) * d; // Won't overflow
  bottom_half >>=
      64; // Only need the top 64 bits, as we'll shift the lower half away;
  __uint128_t top_half = (lowbits >> 64) * d;
  __uint128_t both_halves =
      bottom_half + top_half; // Both halves are already shifted down by 64
  both_halves >>= 64;         // Get top half of both_halves
  return (uint64_t)both_halves;
}

// What follows is the 64-bit functions.
// They are currently not supported on Visual Studio
// due to the lack of a __dwp_mul128_u64 function.
// They may not be faster than what the compiler
// can produce.

static inline __uint128_t
__dwp_computeM_u64(uint64_t d)
{
  // what follows is just ((__uint128_t)0 - 1) / d) + 1 spelled out
  __uint128_t M = UINT64_C(0xFFFFFFFFFFFFFFFF);
  M <<= 64;
  M |= UINT64_C(0xFFFFFFFFFFFFFFFF);
  M /= d;
  M += 1;
  return M;
}

static inline uint64_t
__dwp_fastmod_u64(uint64_t a, __uint128_t M, uint64_t d)
{
  __uint128_t lowbits = M * a;
  return __dwp_mul128_u64(lowbits, d);
}

static inline uint64_t
__dwp_fastdiv_u64(uint64_t a, __uint128_t M)
{
  return __dwp_mul128_u64(M, a);
}

// End of the 64-bit functions

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
#ifndef __unset
#define __unset(flags, index)                                                 \
  ((flags)[(index) / 64] &= ~(1LLU << ((index) % 64)))
#endif

#define __hash_eq_number(a, b) ((a) == (b))
#define __hash_eq_string(a, b) (strcmp((a), (b)) == 0)
#define __hash_hash_u32(key) __lh3_Jenkins_hash_int(key)
#define __hash_hash_u64(key) __lh3_Jenkins_hash_64(key)
#define __hash_hash_string(key) __string_hashcode(key)

#define define_hashtable_entry(name, ktype, vtype)                            \
  typedef struct {                                                            \
    ktype key;                                                                \
    vtype value;                                                              \
  } hashtable_##name##_entry_t;

#define define_hashset_entry(name, ktype)                                     \
  typedef struct {                                                            \
    ktype key;                                                                \
  } hashset_##name##_entry_t;

define_hashset_entry(i, int);
define_hashset_entry(l, int64_t);
define_hashset_entry(s, char*);

define_hashtable_entry(ii, int, int);
define_hashtable_entry(ll, int64_t, int64_t);
define_hashtable_entry(si, char*, int);
define_hashtable_entry(ss, char*, char*);
define_hashtable_entry(li, int64_t, int);

#endif // __hash_base_h__
