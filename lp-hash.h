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
      uint64_t block_size;                                                    \
    } raw;                                                                    \
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

#define __define_lphash_method(name, ktype, feq, fhash)                       \
  static inline lphash##name##_t* lphash##name##_with_capacity(               \
      uint64_t capacity, uint64_t block_size, float load_factor)              \
  {                                                                           \
    lphash##name##_t* table =                                                 \
        (lphash##name##_t*)hash_malloc(sizeof(lphash##name##_t));             \
    table->size = 0;                                                          \
    table->load_factor = load_factor;                                         \
    table->raw.block_size = block_size;                                       \
    capacity = 1.0 * capacity / load_factor;                                  \
    capacity = __hash_roundup64_dwp__(capacity);                              \
    table->raw.capacity = capacity;                                           \
    table->raw.mask = table->raw.capacity - 1;                                \
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
    }                                                                         \
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
    for (int i = 0; i < table->raw.nblocks; i++) {                            \
      __lphash##name##_raw_entries_t* block = table->raw.entries[i];          \
      for (uint64_t j = 0; j < block->offset; j++) {                          \
        h = fhash(block->entries[j].key) % mask;                              \
        while (__is_set(table->flags, h)) {                                   \
          h = (h + 1) & mask;                                                 \
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
    }                                                                         \
    table->raw.nblocks = nblocks;                                             \
    table->raw.capacity = capacity;                                           \
    table->raw.mask = capacity - 1;                                           \
  }                                                                           \
  static inline hash##name##_entry_t* lphash##name##_put(                     \
      lphash##name##_t* table, hash##name##_entry_t* entry, int replace,      \
      int* exist)                                                             \
  {                                                                           \
    if (1.0 * table->size / table->raw.capacity > table->load_factor) {       \
      lphash##name##_resize(table);                                           \
    }                                                                         \
    *exist = 0;                                                               \
    uint64_t h;                                                               \
    uint64_t mask = table->raw.mask;                                          \
    h = fhash(entry->key) % mask;                                             \
    while (__is_set(table->flags, h)) {                                       \
      if (feq(table->raw.entries[table->entries[h] / table->raw.block_size]   \
                  ->entries[table->entries[h] % table->raw.block_size]        \
                  .key,                                                       \
              entry->key)) {                                                  \
        *exist = 1;                                                           \
        if (replace) {                                                        \
          table->raw.entries[table->entries[h] / table->raw.block_size]       \
              ->entries[table->entries[h] % table->raw.block_size] = *entry;  \
        }                                                                     \
        return &table->raw.entries[table->entries[h] / table->raw.block_size] \
                    ->entries[table->entries[h] % table->raw.block_size];     \
      }                                                                       \
      h = (h + 1) & mask;                                                     \
    }                                                                         \
    __set(table->flags, h);                                                   \
    __lphash##name##_raw_entries_t* block =                                   \
        table->raw.entries[table->raw.block_index];                           \
    table->entries[h] =                                                       \
        table->raw.block_index * table->raw.block_size + block->offset;       \
    block->entries[block->offset] = *entry;                                   \
    block->offset++;                                                          \
    table->size++;                                                            \
    if (block->offset == block->capacity) {                                   \
      table->raw.block_index++;                                               \
    }                                                                         \
    return &table->raw.entries[table->entries[h] / table->raw.block_size]     \
                ->entries[table->entries[h] % table->raw.block_size];         \
  }                                                                           \
                                                                              \
  static inline hash##name##_entry_t* lphash##name##_get(                     \
      lphash##name##_t* table, ktype key, int* exist)                         \
  {                                                                           \
    *exist = 0;                                                               \
    uint64_t h = fhash(key) % table->raw.mask;                                \
    while (__is_set(table->flags, h)) {                                       \
      if (feq(table->raw.entries[table->entries[h] / table->raw.block_size]   \
                  ->entries[table->entries[h] % table->raw.block_size]        \
                  .key,                                                       \
              key)) {                                                         \
        *exist = 1;                                                           \
        return &table->raw.entries[table->entries[h] / table->raw.block_size] \
                    ->entries[table->entries[h] % table->raw.block_size];     \
      }                                                                       \
      h = (h + 1) & table->raw.mask;                                          \
    }                                                                         \
    return NULL;                                                              \
  }

#define define_lphashtable(name, ktype, vtype, feq, fhash)                    \
  __define_hash_table_entry(table_##name, ktype, vtype);                      \
  __define_lphash(table_##name, ktype, vtype);                                \
  __define_lphash_method(table_##name, ktype, feq, fhash)

#define define_lphashset(name, ktype, feq, fhash)                             \
  __define_hash_set_entry(set_##name, ktype);                                 \
  __define_lphash(set_##name, ktype, ktype);                                  \
  __define_lphash_method(set_##name, ktype, feq, fhash)

// clang-format off
#define lphashtable_with_capacity(name, capacity, block_size, load)   lphashtable_##name##_with_capacity(capacity, block_size, load)
#define lphashtable_free(name, table)                                 lphashtable_##name##_free(table)
#define lphashtable_get(name, table, key, found)                      lphashtable_##name##_get(table, key, found)
#define lphashtable_put(name, table, entry)                           lphashtable_##name##_put(table, entry)
// clang-format on

// clang-format off
#define lphashset_with_capacity(name, capacity, block_size, load)     lphashset_##name##_with_capacity(capacity, block_size, load)
#define lphashset_free(name, table)                                   lphashset_##name##_free(table)
#define lphashset_get(name, table, key, found)                        lphashset_##name##_get(table, key, found)
#define lphashset_put(name, table, entry)                             lphashset_##name##_put(table, entry)
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

define_lphashtable(ii, int, int, __hash_eq_number, __hash_hash_u32);
define_lphashtable(ll, int64_t, int64_t, __hash_eq_number, __hash_hash_u64);
define_lphashtable(si, char*, int, __hash_eq_string, __hash_hash_string);
define_lphashtable(ss, char*, char*, __hash_eq_string, __hash_hash_string);
define_lphashtable(li, int64_t, int, __hash_eq_number, __hash_hash_u64);

define_lphashset(i, int, __hash_eq_number, __hash_hash_u32);
define_lphashset(l, int64_t, __hash_eq_number, __hash_hash_u64);
define_lphashset(s, char*, __hash_eq_string, __hash_hash_string);

#ifdef __cplusplus
}
#endif

#endif // __lp_hash_h__
