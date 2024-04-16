#ifndef __hash_h__
#define __hash_h__

#include "hash-base.h"
#include "lp-hash.h"

static const uint64_t sys_prime_list[61] = {
  0x7LLU,
  0xfLLU,
  0x1fLLU,
  0x43LLU,
  0x89LLU,
  0x115LLU,
  0x22dLLU,
  0x45dLLU,
  0x8bdLLU,
  0x1181LLU,
  0x2303LLU,
  0x4609LLU,
  0x8c17LLU,
  0x1183dLLU,
  0x2307bLLU,
  0x460fdLLU,
  0x8c201LLU,
  0x118411LLU,
  0x230833LLU,
  0x461069LLU,
  0x8c20e1LLU,
  0x11841cbLLU,
  0x2308397LLU,
  0x461075bLLU,
  0x8c20ecbLLU,
  0x11841da5LLU,
  0x23083b61LLU,
  0x461076c7LLU,
  0x8c20ed91LLU,
  0x11841db31LLU,
  0x23083b673LLU,
  0x461076d1bLLU,
  0x8c20eda41LLU,
  0x11841db48dLLU,
  0x23083b6937LLU,
  0x461076d27fLLU,
  0x8c20eda50dLLU,
  0x11841db4a59LLU,
  0x23083b694ebLLU,
  0x461076d29f1LLU,
  0x8c20eda5441LLU,
  0x11841db4a887LLU,
  0x23083b69511fLLU,
  0x461076d2a2c1LLU,
  0x8c20eda54591LLU,
  0x11841db4a8b55LLU,
  0x23083b69516c1LLU,
  0x461076d2a2da5LLU,
  0x8c20eda545b55LLU,
  0x11841db4a8b6b5LLU,
  0x23083b69516d91LLU,
  0x461076d2a2db3bLLU,
  0x8c20eda545b69dLLU,
  0x11841db4a8b6d5dLLU,
  0x23083b69516daf5LLU,
  0x461076d2a2db5edLLU,
  0x8c20eda545b6c5fLLU,
  0x11841db4a8b6d8ebLLU,
  0x23083b69516db1ffLLU,
  0x461076d2a2db643fLLU,
  0x8c20eda545b6c8f3LLU,
};

static inline void
__hash_prime(uint64_t size, int* n)
{
  for (int i = 0; i < 61; i++) {
    if (sys_prime_list[i] > size) {
      *n = i;
      return;
    }
  }
}

static inline uint64_t
__hash_prime_bigger(uint64_t size)
{
  for (int i = 0; i < 61; i++) {
    if (sys_prime_list[i] > size) {
      return sys_prime_list[i];
    }
  }
  return 0;
}

#define __define_hash(name, ktype, vtype, feq, fhash)                         \
  typedef struct {                                                            \
    uint64_t mask;                                                            \
    uint64_t prime;                                                           \
    uint64_t size;                                                            \
    uint64_t* flags;                                                          \
    hash##name##_entry_t* entries;                                            \
  } _hash##name##_array_t;                                                    \
  typedef struct {                                                            \
    uint64_t size;                                                            \
    int m;                                                                    \
    float load;                                                               \
    _hash##name##_array_t* array;                                             \
    lphash##name##_t* linear;                                                 \
    struct {                                                                  \
      uint64_t offset;                                                        \
      uint64_t size;                                                          \
      int status;                                                             \
      int m;                                                                  \
      int type;                                                               \
    } iter;                                                                   \
  } hash##name##_t;

#define __define_hash_method(name, feq, fhash, ktype, vtype)                  \
  static inline hash##name##_t* hash##name##_with_capacity(uint64_t capacity, \
                                                           int m, float load) \
  {                                                                           \
    assert(load > 0 && load < 1);                                             \
    capacity = capacity < 128 ? 128 : capacity;                               \
    m = m < 2 ? 2 : m;                                                        \
    int n = 0;                                                                \
    __hash_prime(capacity, &n);                                               \
    uint64_t size_list[60] = { 0 };                                           \
    if (m > n) {                                                              \
      m = n;                                                                  \
    }                                                                         \
    int i = 0;                                                                \
    while (n > 0 && i < m) {                                                  \
      size_list[i] = sys_prime_list[n] + 1;                                   \
      n--;                                                                    \
      i++;                                                                    \
    }                                                                         \
    hash##name##_t* table =                                                   \
        (hash##name##_t*)hash_malloc(sizeof(hash##name##_t));                 \
    table->size = 0;                                                          \
    table->m = m;                                                             \
    table->load = load;                                                       \
    table->array = (_hash##name##_array_t*)hash_malloc(                       \
        sizeof(_hash##name##_array_t) * m);                                   \
    for (int i = 0; i < m; i++) {                                             \
      table->array[i].mask = size_list[i] - 1;                                \
      table->array[i].prime = size_list[i];                                   \
      table->array[i].size = 0;                                               \
      table->array[i].flags = (uint64_t*)hash_malloc(                         \
          sizeof(uint64_t) * (size_list[i] + 63) / 64);                       \
      memset(table->array[i].flags, 0,                                        \
             sizeof(uint64_t) * (size_list[i] + 63) / 64);                    \
      table->array[i].entries = (hash##name##_entry_t*)hash_malloc(           \
          sizeof(hash##name##_entry_t) * size_list[i]);                       \
    }                                                                         \
    table->linear = lphash##name##_with_capacity(1024, 1024 * 128, load);     \
    table->iter.offset = 0;                                                   \
    table->iter.status = 0;                                                   \
    table->iter.m = 0;                                                        \
    table->iter.type = 0;                                                     \
    table->iter.size = 0;                                                     \
    return table;                                                             \
  }                                                                           \
  static inline void hash##name##_free(hash##name##_t* table)                 \
  {                                                                           \
    for (int i = 0; i < table->m; i++) {                                      \
      hash_free(table->array[i].entries);                                     \
      hash_free(table->array[i].flags);                                       \
    }                                                                         \
    hash_free(table->array);                                                  \
    lphash##name##_free(table->linear);                                       \
    hash_free(table);                                                         \
  }                                                                           \
  static inline hash##name##_entry_t* hash##name##_get(hash##name##_t* table, \
                                                       ktype key, int* found) \
  {                                                                           \
    uint64_t h = fhash(key);                                                  \
    _hash##name##_array_t* array = table->array;                              \
    {                                                                         \
      hash##name##_entry_t* entries;                                          \
      *found = 0;                                                             \
      for (int i = 0; i < table->m; i++) {                                    \
        if (!array[i].size) {                                                 \
          continue;                                                           \
        }                                                                     \
        uint64_t index = h % array[i].mask;                                   \
        if (!__is_set(array[i].flags, index)) {                               \
          *found = 0;                                                         \
          return NULL;                                                        \
        }                                                                     \
        entries = array[i].entries;                                           \
        if (feq(entries[index].key, key)) {                                   \
          *found = 1;                                                         \
          return &entries[index];                                             \
        }                                                                     \
      }                                                                       \
    }                                                                         \
    if (table->linear->size == 0) {                                           \
      return NULL;                                                            \
    }                                                                         \
    return lphash##name##_get(table->linear, key, found);                     \
  }                                                                           \
  static inline hash##name##_entry_t* hash##name##_put(                       \
      hash##name##_t* table, hash##name##_entry_t* entry, int replace,        \
      int* exist)                                                             \
  {                                                                           \
    uint64_t h = fhash(entry->key);                                           \
    _hash##name##_array_t* array_list = table->array;                         \
    hash##name##_entry_t* entries;                                            \
    ktype key = entry->key;                                                   \
    *exist = 0;                                                               \
    for (int i = 0; i < table->m; i++) {                                      \
      entries = array_list[i].entries;                                        \
      uint64_t index = h % array_list[i].mask;                                \
      if (__is_set(array_list[i].flags, index)) {                             \
        if (feq(entries[index].key, key)) {                                   \
          if (replace) {                                                      \
            memcpy(entries + index, entry, sizeof(hash##name##_entry_t));     \
          }                                                                   \
          *exist = 1;                                                         \
          return &entries[index];                                             \
        }                                                                     \
        continue;                                                             \
      }                                                                       \
      table->size++;                                                          \
      array_list[i].size++;                                                   \
      memcpy(entries + index, entry, sizeof(hash##name##_entry_t));           \
      __set(array_list[i].flags, index);                                      \
      return &entries[index];                                                 \
    }                                                                         \
    entries = lphash##name##_put(table->linear, entry, replace, exist);       \
    if (!*exist) {                                                            \
      table->size++;                                                          \
    }                                                                         \
    return entries;                                                           \
  }                                                                           \
  static inline hash##name##_entry_t* hash##name##_iter(                      \
      hash##name##_t* table)                                                  \
  {                                                                           \
    if (table->iter.status == 1) {                                            \
      return NULL;                                                            \
    }                                                                         \
    if (table->iter.size == table->size || !table->size) {                    \
      table->iter.status = 1;                                                 \
      return NULL;                                                            \
    }                                                                         \
    if (table->iter.type == 0) {                                              \
      while (1) {                                                             \
        if (table->m == table->iter.m) {                                      \
          table->iter.type = 1;                                               \
          table->iter.offset = 0;                                             \
          break;                                                              \
        }                                                                     \
        _hash##name##_array_t* array = &table->array[table->iter.m];          \
        hash##name##_entry_t* entries = table->array[table->iter.m].entries;  \
        while (!__is_set(array->flags, table->iter.offset)                    \
               && table->iter.offset < array->prime) {                        \
          table->iter.offset++;                                               \
        }                                                                     \
        if (table->iter.offset == array->prime) {                             \
          table->iter.m++;                                                    \
          table->iter.offset = 0;                                             \
          continue;                                                           \
        }                                                                     \
        table->iter.size++;                                                   \
        return &entries[table->iter.offset++];                                \
      }                                                                       \
    }                                                                         \
    if (!table->linear->size) {                                               \
      table->iter.status = 1;                                                 \
      return NULL;                                                            \
    }                                                                         \
    hash##name##_entry_t* entry = lphash##name##_iter(table->linear);         \
    if (entry) {                                                              \
      table->iter.size++;                                                     \
    } else {                                                                  \
      table->iter.status = 1;                                                 \
    }                                                                         \
    return entry;                                                             \
  }

#define _define_hashtable(name, ktype, vtype, feq, fhash)                     \
  __define_hash(table_##name, ktype, vtype, feq, fhash);                      \
  __define_hash_method(table_##name, feq, fhash, ktype, vtype);

// clang-format off
#define hashtable_with_capacity(name, max_size, m, load) hashtable_##name##_with_capacity(max_size, m, load)
#define hashtable_free(name, table)              hashtable_##name##_free(table)
#define hashtable_get(name, table, key, found)   hashtable_##name##_get(table, key, found)
#define hashtable_put(name, table, entry)        hashtable_##name##_put(table, entry)
#define hashtable_iter(name, table)              hashtable_##name##_iter(table)
// clang-format on

#define _define_hashset(name, ketype, feq, fhash)                             \
  __define_hash(set_##name, ketype, NULL, feq, fhash);                        \
  __define_hash_method(set_##name, feq, fhash, ketype, NULL);

// clang-format off
#define hashset_with_capacity(name, max_size, m, load) hashset_##name##_with_capacity(max_size, m, load)
#define hashset_free(name, table)                hashset_##name##_free(table)
#define hashset_get(name, table, key, found)     hashset_##name##_get(table, key, found)
#define hashset_put(name, table, entry)          hashset_##name##_put(table, entry)
#define hashset_iter(name, table)                hashset_##name##_iter(table)
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

_define_hashtable(ii, int, int, __hash_eq_number, __hash_hash_u32);
_define_hashtable(ll, int64_t, int64_t, __hash_eq_number, __hash_hash_u64);
_define_hashtable(si, char*, int, __hash_eq_string, __hash_hash_string);
_define_hashtable(ss, char*, char*, __hash_eq_string, __hash_hash_string);
_define_hashtable(li, int64_t, int, __hash_eq_number, __hash_hash_u64);

_define_hashset(i, int, __hash_eq_number, __hash_hash_u32);
_define_hashset(l, int64_t, __hash_eq_number, __hash_hash_u64);
_define_hashset(s, char*, __hash_eq_string, __hash_hash_string);

#ifdef __cplusplus
}
#endif

#define define_hash(name, ktype, vtype, feq, fhash)                           \
  define_hashtable_entry(name, ktype, vtype);                                 \
  define_hashset_entry(name, ktype);                                          \
  _define_lphashtable(name, ktype, vtype, feq, fhash);                         \
  _define_lphashset(name, ktype, feq, fhash);                                  \
  _define_hashtable(name, ktype, vtype, feq, fhash);                           \
  _define_hashset(name, ktype, feq, fhash);

#define define_hashtable(name, ktype, vtype, feq, fhash)                      \
  define_hashtable_entry(name, ktype, vtype);                                 \
  _define_lphashtable(name, ktype, vtype, feq, fhash);                        \
  _define_hashtable(name, ktype, vtype, feq, fhash)

#define define_hashset(name, ktype, feq, fhash)                               \
  define_hashset_entry(name, ktype);                                          \
  _define_lphashset(name, ktype, feq, fhash);                                 \
  _define_hashset(name, ktype, feq, fhash)

#endif
