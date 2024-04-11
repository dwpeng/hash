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

#if defined(HASH_MMAP) && defined(__linux__)
#include <sys/mman.h>
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif
#ifndef MAP_ANON
#define MAP_ANON 0x20
#endif
#define hmalloc(size)                                                         \
  mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
#define hfree(ptr, size) munmap(ptr, size)
#else
#define hmalloc(size) hash_malloc(size)
#define hfree(ptr, size) hash_free(ptr)
#endif

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

#define __define_hash(name, ktype, vtype)                                     \
  typedef struct {                                                            \
    uint64_t size;                                                            \
    uint64_t capacity;                                                        \
    uint64_t* flags;                                                          \
    struct {                                                                  \
      hash##name##_entry_t* entry;                                            \
      uint64_t offset;                                                        \
      uint64_t capacity;                                                      \
    } raw;                                                                    \
    uint32_t* entries;                                                        \
  } hash##name##_t;

// for development
// ---------------------------------------
typedef int ktype;
typedef int vtype;

typedef uint64_t (*feq)(ktype, ktype);
typedef uint64_t (*fhash)(ktype);
__define_hash_table_entry(xxxxx, ktype, vtype);
__define_hash(xxxxx, ktype, vtype);
// ---------------------------------------

