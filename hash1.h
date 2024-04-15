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

#define LOAD 0.75 // default load factor

#define __define_hash_table_entry(name, ktype, vtype)                         \
  typedef struct {                                                            \
    ktype key;                                                                \
    vtype value;                                                              \
  } hash##name##_entry_t;

#define __define_hash_set_entry(name, ktype)                                  \
  typedef struct {                                                            \
    ktype key;                                                                \
  } hash##name##_entry_t;

#define __is_set(flags, index) (((flags)[(index) / 64] >> ((index) % 64)) & 1)
#define __set(flags, index) ((flags)[(index) / 64] |= (1LLU << ((index) % 64)))

#define OFFSET_BITS 24
#define MAX_OFFSET_SIZE (1 << OFFSET_BITS)

#define __define_hash(name, ktype, vtype)                                     \
  typedef struct __hash##name##raw_entries_t __hash##name##raw_entries_t;     \
  struct __hash##name##raw_entries_t {                                        \
    hash##name##_entry_t* entries;                                            \
    uint64_t capacity;                                                        \
    uint64_t offset;                                                          \
  };                                                                          \
  typedef struct {                                                            \
    uint64_t size;                                                            \
    float load_factor;                                                        \
    uint64_t* flags;                                                          \
    struct {                                                                  \
      __hashxxxxxraw_entries_t** entries;                                     \
      int block_index;                                                        \
      int nblocks;                                                            \
      uint64_t capacity;                                                      \
    } raw;                                                                    \
    uint32_t* entries;                                                        \
  } hash##name##_t;

// memory layout
// flags: 64 bits * length per entry, 1 bit per entry
// raw
//            +------+------+------+------+------+
//  entries ->|  0   |   1  |   2  |      | .... |  -> i
//            +------+------+------+------+------+
//                              |
//                         +--------+--------+
//                         |   0    |   1    | .... -> j
//                         +--------+--------+
//                _____________|
// entries       ||     0
//            +--||--+------+------+------+------+
//  entries ->|offset|      |      |      | .... |  -> i * MAX_OFFSET_SIZE + j
//            +------+------+------+------+------+

// for development
// ---------------------------------------
typedef int ktype;
typedef int vtype;

static inline uint64_t
feq(ktype a, ktype b)
{
  return a == b;
}

static inline uint64_t
fhash(ktype key)
{
  return key;
}

__define_hash_table_entry(xxxxx, ktype, vtype);
__define_hash(xxxxx, ktype, vtype);
// ---------------------------------------
