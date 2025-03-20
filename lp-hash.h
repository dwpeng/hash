#ifndef __lp_hash_h__
#define __lp_hash_h__

#include "hash-base.h"

#define __define_lphash(name, ktype, vtype)                                   \
  typedef struct __lphash##name##_raw_entries_t                               \
      __lphash##name##_raw_entries_t;                                         \
  struct __lphash##name##_raw_entries_t {                                     \
    hash##name##_entry_t* entries;                                            \
    uint64_t capacity;                                                        \
    uint64_t offset;                                                          \
    uint64_t* flags;                                                          \
  };                                                                          \
  typedef struct {                                                            \
    uint64_t size;                                                            \
    float load_factor;                                                        \
    uint64_t* flags;                                                          \
    struct {                                                                  \
      __lphash##name##_raw_entries_t** entries;                               \
      int block_index;                                                        \
      int nblocks;                                                            \
      uint64_t capacity;                                                      \
      uint64_t mask;                                                          \
      __uint128_t M;                                                          \
      uint64_t block_size;                                                    \
      __uint128_t blockM;                                                     \
    } raw;                                                                    \
    struct {                                                                  \
      int block_index;                                                        \
      uint64_t offset;                                                        \
    } iter;                                                                   \
    uint32_t* entries;                                                        \
  } lphash##name##_t

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

#define __define_lphash_method(name, ktype, feq, fhash, fentrykey)            \
  static inline lphash##name##_t* lphash##name##_with_capacity(               \
      uint64_t capacity, uint64_t block_size, float load_factor)              \
  {                                                                           \
    lphash##name##_t* table =                                                 \
        (lphash##name##_t*)hash_malloc(sizeof(lphash##name##_t));             \
    table->size = 0;                                                          \
    table->load_factor = load_factor;                                         \
    table->raw.block_size = block_size;                                       \
    assert(block_size > 1);                                                   \
    table->raw.blockM = __dwp_computeM_u64(block_size);                       \
    capacity = 1.0 * capacity / load_factor;                                  \
    capacity = __hash_roundup64_dwp__(capacity);                              \
    table->raw.capacity = capacity;                                           \
    table->raw.mask = table->raw.capacity - 1ULL;                             \
    table->raw.M = __dwp_computeM_u64(capacity - 1ULL);                       \
    table->raw.block_index = 0;                                               \
    int nblocks = capacity / block_size + 1;                                  \
    table->raw.nblocks = nblocks;                                             \
    table->raw.entries = (__lphash##name##_raw_entries_t**)hash_malloc(       \
        sizeof(__lphash##name##_raw_entries_t*) * nblocks);                   \
    for (int i = 0; i < nblocks; i++) {                                       \
      table->raw.entries[i] = (__lphash##name##_raw_entries_t*)hash_malloc(   \
          sizeof(__lphash##name##_raw_entries_t) * block_size);               \
      table->raw.entries[i]->capacity = block_size;                           \
      table->raw.entries[i]->offset = 0;                                      \
      table->raw.entries[i]->entries = (hash##name##_entry_t*)hash_malloc(    \
          sizeof(hash##name##_entry_t) * block_size);                         \
      table->raw.entries[i]->flags =                                          \
          (uint64_t*)hash_malloc(sizeof(uint64_t) * (block_size + 63) / 64);  \
    }                                                                         \
    table->iter.block_index = 0;                                              \
    table->iter.offset = 0;                                                   \
    table->flags =                                                            \
        (uint64_t*)hash_malloc(sizeof(uint64_t) * (capacity + 63) / 64);      \
    memset(table->flags, 0, sizeof(uint64_t) * (capacity + 63) / 64);         \
    table->entries = (uint32_t*)hash_malloc(sizeof(uint32_t) * capacity);     \
    return table;                                                             \
  }                                                                           \
  static inline void lphash##name##_free(lphash##name##_t* table)             \
  {                                                                           \
    for (int i = 0; i < table->raw.nblocks; i++) {                            \
      hash_free(table->raw.entries[i]->entries);                              \
      hash_free(table->raw.entries[i]->flags);                                \
      hash_free(table->raw.entries[i]);                                       \
    }                                                                         \
    hash_free(table->raw.entries);                                            \
    hash_free(table->flags);                                                  \
    hash_free(table->entries);                                                \
    hash_free(table);                                                         \
  }                                                                           \
  static inline void lphash##name##_clear(lphash##name##_t* table)            \
  {                                                                           \
    memset(table->flags, 0,                                                   \
           sizeof(uint64_t) * (table->raw.capacity + 63) / 64);               \
    table->size = 0;                                                          \
    table->raw.block_index = 0;                                               \
    table->iter.block_index = 0;                                              \
    table->iter.offset = 0;                                                   \
    for (int i = 0; i < table->raw.nblocks; i++) {                            \
      table->raw.entries[i]->offset = 0;                                      \
      memset(table->raw.entries[i]->flags, 0,                                 \
             sizeof(uint64_t) * (table->raw.entries[i]->capacity + 63) / 64); \
    }                                                                         \
  }                                                                           \
  static inline void lphash##name##_resize(lphash##name##_t* table)           \
  {                                                                           \
    float load_factor = table->load_factor;                                   \
    uint64_t block_size = table->raw.block_size;                              \
    uint64_t capacity = table->raw.capacity;                                  \
    capacity = 1.0 * capacity / load_factor;                                  \
    capacity = __hash_roundup64_dwp__(capacity);                              \
    int nblocks = capacity / block_size + 1;                                  \
    table->raw.entries = (__lphash##name##_raw_entries_t**)hash_realloc(      \
        table->raw.entries,                                                   \
        sizeof(__lphash##name##_raw_entries_t*) * nblocks);                   \
    table->flags = (uint64_t*)hash_realloc(                                   \
        table->flags, sizeof(uint64_t) * (capacity + 63) / 64);               \
    memset(table->flags, 0, sizeof(uint64_t) * (capacity + 63) / 64);         \
    table->entries =                                                          \
        (uint32_t*)hash_realloc(table->entries, sizeof(uint32_t) * capacity); \
    uint64_t h;                                                               \
    uint64_t mask = capacity - 1;                                             \
    __uint128_t M = __dwp_computeM_u64(mask);                                 \
    for (int i = 0; i < table->raw.nblocks; i++) {                            \
      __lphash##name##_raw_entries_t* block = table->raw.entries[i];          \
      for (uint64_t j = 0; j < block->offset; j++) {                          \
        h = fhash(block->entries[j].key);                                     \
        h = __dwp_fastmod_u64(h, M, mask);                                    \
        while (__is_set(table->flags, h)) {                                   \
          h = __dwp_fastmod_u64(h + 1, M, mask);                              \
        }                                                                     \
        __set(table->flags, h);                                               \
        table->entries[h] = i * block_size + j;                               \
      }                                                                       \
    }                                                                         \
    for (int i = table->raw.nblocks; i < nblocks; i++) {                      \
      table->raw.entries[i] = (__lphash##name##_raw_entries_t*)hash_malloc(   \
          sizeof(__lphash##name##_raw_entries_t) * block_size);               \
      table->raw.entries[i]->capacity = block_size;                           \
      table->raw.entries[i]->offset = 0;                                      \
      table->raw.entries[i]->entries = (hash##name##_entry_t*)hash_malloc(    \
          sizeof(hash##name##_entry_t) * block_size);                         \
      table->raw.entries[i]->flags =                                          \
          (uint64_t*)hash_malloc(sizeof(uint64_t) * (block_size + 63) / 64);  \
    }                                                                         \
    table->raw.nblocks = nblocks;                                             \
    table->raw.capacity = capacity;                                           \
    table->raw.mask = capacity - 1;                                           \
    table->raw.M = M;                                                         \
  }                                                                           \
  static inline hash##name##_entry_t* lphash##name##_hput(                    \
      lphash##name##_t* table, hash##name##_entry_t* entry, uint64_t h,       \
      int replace, int* exist)                                                \
  {                                                                           \
    if (1.0 * table->size / table->raw.capacity > table->load_factor) {       \
      lphash##name##_resize(table);                                           \
    }                                                                         \
    *exist = 0;                                                               \
    uint64_t mask = table->raw.mask;                                          \
    h = __dwp_fastmod_u64(h, table->raw.M, mask);                             \
    uint64_t block_index;                                                     \
    uint64_t block_offset;                                                    \
    while (__is_set(table->flags, h)) {                                       \
      block_index = __dwp_fastdiv_u64(table->entries[h], table->raw.blockM);  \
      block_offset = __dwp_fastmod_u64(table->entries[h], table->raw.blockM,  \
                                       table->raw.block_size);                \
      if (feq(fentrykey(table->raw.entries[block_index]                       \
                            ->entries[block_offset]                           \
                            .key),                                            \
              fentrykey(entry->key))) {                                       \
        *exist = 1;                                                           \
        if (replace) {                                                        \
          table->raw.entries[block_index]->entries[block_offset] = *entry;    \
          __set(table->raw.entries[block_index]->flags, block_offset);        \
        }                                                                     \
        return &table->raw.entries[block_index]->entries[block_offset];       \
      }                                                                       \
      h = __dwp_fastmod_u64(h + 1, table->raw.M, mask);                       \
    }                                                                         \
    __set(table->flags, h);                                                   \
    __lphash##name##_raw_entries_t* block =                                   \
        table->raw.entries[table->raw.block_index];                           \
    table->entries[h] =                                                       \
        table->raw.block_index * table->raw.block_size + block->offset;       \
    block->entries[block->offset] = *entry;                                   \
    __set(block->flags, block->offset);                                       \
    block->offset++;                                                          \
    table->size++;                                                            \
    if (block->offset == block->capacity) {                                   \
      table->raw.block_index++;                                               \
    }                                                                         \
    block_index = __dwp_fastdiv_u64(table->entries[h], table->raw.blockM);    \
    block_offset = __dwp_fastmod_u64(table->entries[h], table->raw.blockM,    \
                                     table->raw.block_size);                  \
    return &table->raw.entries[block_index]->entries[block_offset];           \
  }                                                                           \
  static inline hash##name##_entry_t* lphash##name##_put(                     \
      lphash##name##_t* table, hash##name##_entry_t* entry, int replace,      \
      int* exist)                                                             \
  {                                                                           \
    return lphash##name##_hput(table, entry, fhash(fentrykey(entry->key)),    \
                               replace, exist);                               \
  }                                                                           \
  static inline hash##name##_entry_t* lphash##name##_get(                     \
      lphash##name##_t* table, ktype key, int* exist)                         \
  {                                                                           \
    *exist = 0;                                                               \
    uint64_t h = fhash(key);                                                  \
    uint64_t mask = table->raw.mask;                                          \
    uint64_t rawh = h;                                                        \
    h = __dwp_fastmod_u64(h, table->raw.M, mask);                             \
    assert(h == rawh % mask);                                                 \
    uint64_t block_index;                                                     \
    uint64_t block_offset;                                                    \
    while (__is_set(table->flags, h)) {                                       \
      block_index = __dwp_fastdiv_u64(table->entries[h], table->raw.blockM);  \
      block_offset = __dwp_fastmod_u64(table->entries[h], table->raw.blockM,  \
                                       table->raw.block_size);                \
      assert(table->entries[h] % table->raw.block_size == block_offset);      \
      if (feq(fentrykey(table->raw.entries[block_index]                       \
                            ->entries[block_offset]                           \
                            .key),                                            \
              key)) {                                                         \
        *exist = 1;                                                           \
        return &table->raw.entries[block_index]->entries[block_offset];       \
      }                                                                       \
      h = __dwp_fastmod_u64(h + 1, table->raw.M, mask);                       \
    }                                                                         \
    return NULL;                                                              \
  }                                                                           \
  static inline int lphash##name##_del(lphash##name##_t* table, ktype key)    \
  {                                                                           \
    uint64_t h = fhash(key);                                                  \
    uint64_t mask = table->raw.mask;                                          \
    h = __dwp_fastmod_u64(h, table->raw.M, mask);                             \
    uint64_t block_index;                                                     \
    uint64_t block_offset;                                                    \
    while (__is_set(table->flags, h)) {                                       \
      block_index = __dwp_fastdiv_u64(table->entries[h], table->raw.blockM);  \
      block_offset = __dwp_fastmod_u64(table->entries[h], table->raw.blockM,  \
                                       table->raw.block_size);                \
      if (feq(fentrykey(table->raw.entries[block_index]                       \
                            ->entries[block_offset]                           \
                            .key),                                            \
              key)) {                                                         \
        __unset(table->flags, h);                                             \
        __unset(table->raw.entries[block_index]->flags, block_offset);        \
        table->size--;                                                        \
        return 1;                                                             \
      }                                                                       \
      h = __dwp_fastmod_u64(h + 1, table->raw.M, mask);                       \
    }                                                                         \
    return 0;                                                                 \
  }                                                                           \
  static inline hash##name##_entry_t* lphash##name##_iter(                    \
      lphash##name##_t* table)                                                \
  {                                                                           \
    if (table->iter.block_index >= table->raw.nblocks) {                      \
      return NULL;                                                            \
    }                                                                         \
    __lphash##name##_raw_entries_t* block =                                   \
        table->raw.entries[table->iter.block_index];                          \
    if (table->iter.offset >= block->offset) {                                \
      table->iter.block_index++;                                              \
      table->iter.offset = 0;                                                 \
      return lphash##name##_iter(table);                                      \
    }                                                                         \
    while (table->iter.offset < block->offset) {                              \
      if (__is_set(block->flags, table->iter.offset)) {                       \
        return &block->entries[table->iter.offset++];                         \
      }                                                                       \
      table->iter.offset++;                                                   \
    }                                                                         \
    return lphash##name##_iter(table);                                        \
  }                                                                           \
  static inline void lphash##name##_reset_iter(lphash##name##_t* table)       \
  {                                                                           \
    table->iter.block_index = 0;                                              \
    table->iter.offset = 0;                                                   \
  }                                                                           \
  static inline ktype lphash##name##_entrykey(hash##name##_entry_t* entry)    \
  {                                                                           \
    return fentrykey(entry->key);                                             \
  }

#define _define_lphashtable(name, ktype, vtype, feq, fhash, fentrykey)        \
  __define_lphash(table_##name, ktype, vtype);                                \
  __define_lphash_method(table_##name, ktype, feq, fhash, fentrykey)

#define _define_lphashset(name, ktype, feq, fhash, fentrykey)                 \
  __define_lphash(set_##name, ktype, ktype);                                  \
  __define_lphash_method(set_##name, ktype, feq, fhash, fentrykey)

// clang-format off
#define lphashtable_with_capacity(name, capacity, block_size, load)   lphashtable_##name##_with_capacity(capacity, block_size, load)
#define lphashtable_free(name, table)                                 lphashtable_##name##_free(table)
#define lphashtable_get(name, table, key, found)                      lphashtable_##name##_get(table, key, found)
#define lphashtable_put(name, table, entry, replaced, exist)          lphashtable_##name##_put(table, entry, replaced, exist)
#define lphashtable_hput(name, table, entry, h, replaced, exist)      lphashtable_##name##_hput(table, entry, h, replaced, exist)
#define lphashtable_clear(name, table)                                lphashtable_##name##_clear(table)
#define lphashtable_resize(name, table)                               lphashtable_##name##_resize(table)
#define lphashtable_iter(name, table)                                 lphashtable_##name##_iter(table)
#define lphashtable_reset_iter(name, table)                           lphashtable_##name##_reset_iter(table)
#define lphashtable_entrykey(name, entry)                             lphashtable_##name##_entrykey(entry)
// clang-format on

// clang-format off
#define lphashset_with_capacity(name, capacity, block_size, load)     lphashset_##name##_with_capacity(capacity, block_size, load)
#define lphashset_free(name, table)                                   lphashset_##name##_free(table)
#define lphashset_get(name, table, key, found)                        lphashset_##name##_get(table, key, found)
#define lphashset_put(name, table, entry, replaced, exist)            lphashset_##name##_put(table, entry, replaced, exist)
#define lphashset_hput(name, table, entry, h, replaced, exist)        lphashset_##name##_hput(table, entry, h, replaced, exist)
#define lphashset_clear(name, table)                                  lphashset_##name##_clear(table)
#define lphashset_resize(name, table)                                 lphashset_##name##_resize(table)
#define lphashset_iter(name, table)                                   lphashset_##name##_iter(table)
#define lphashset_reset_iter(name, table)                             lphashset_##name##_reset_iter(table)
#define lphashset_entrykey(name, entry)                               lphashset_##name##_entrykey(entry)
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif
// clang-format off
_define_lphashtable(ii, int, int, __hash_eq_number, __hash_hash_u32, __hash_entrykey);
_define_lphashtable(ll, int64_t, int64_t, __hash_eq_number, __hash_hash_u64, __hash_entrykey);
_define_lphashtable(si, char*, int, __hash_eq_string, __hash_hash_string, __hash_entrykey);
_define_lphashtable(ss, char*, char*, __hash_eq_string, __hash_hash_string, __hash_entrykey);
_define_lphashtable(li, int64_t, int, __hash_eq_number, __hash_hash_u64, __hash_entrykey);

_define_lphashset(i, int, __hash_eq_number, __hash_hash_u32, __hash_entrykey);
_define_lphashset(l, int64_t, __hash_eq_number, __hash_hash_u64, __hash_entrykey);
_define_lphashset(s, char*, __hash_eq_string, __hash_hash_string, __hash_entrykey);
// clang-format on
#ifdef __cplusplus
}
#endif

#define define_lphash(name, ktype, vtype, feq, fhash, fentrykey)              \
  define_hashtable_entry(name, ktype, vtype);                                 \
  define_hashset_entry(name, ktype);                                          \
  _define_lphashtable(name, ktype, vtype, feq, fhash, fentrykey);             \
  _define_lphashset(name, ktype, feq, fhash, fentrykey)

#define define_lphashtable(name, ktype, vtype, feq, fhash, fentrykey)         \
  define_hashtable_entry(name, ktype, vtype);                                 \
  _define_lphashtable(name, ktype, vtype, feq, fhash, fentrykey)

#define define_lphashset(name, ktype, feq, fhash, fentrykey)                  \
  define_hashset_entry(name, ktype);                                          \
  _define_lphashset(name, ktype, feq, fhash, fentrykey)

#endif // __lp_hash_h__
