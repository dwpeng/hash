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
    uint64_t capacity;                                                        \
    uint64_t* flags;                                                          \
    hash##name##_entry_t** entries;                                           \
  } _hash##name##_scale_array_t;                                              \
  typedef struct hash##name##_t hash##name##_t;                               \
  struct hash##name##_t {                                                     \
    uint64_t size;                                                            \
    int m;                                                                    \
    float load;                                                               \
    _hash##name##_array_t* array;                                             \
    _hash##name##_scale_array_t* scale_array;                                 \
    struct {                                                                  \
      uint64_t offset;                                                        \
      uint64_t size;                                                          \
      int status;                                                             \
      int m;                                                                  \
      int type;                                                               \
    } iter;                                                                   \
  };

#define __define_hash_method(name, feq, fhash, ktype, vtype)                  \
  static inline hash##name##_t* hash##name##_init_with_load(                  \
      size_t max_size, int m, float load)                                     \
  {                                                                           \
    assert(load > 0 && load < 1);                                             \
    max_size = max_size < 128 ? 128 : max_size;                               \
    m = m < 2 ? 2 : m;                                                        \
    int n = 0;                                                                \
    __hash_prime(max_size, &n);                                               \
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
    table->scale_array = (_hash##name##_scale_array_t*)hash_malloc(           \
        sizeof(_hash##name##_scale_array_t));                                 \
    table->scale_array->entries = (hash##name##_entry_t**)hash_malloc(        \
        sizeof(hash##name##_entry_t*) * 128);                                 \
    table->scale_array->capacity = 128;                                       \
    table->scale_array->flags =                                               \
        (uint64_t*)hash_malloc(sizeof(uint64_t) * (128 + 63) / 64);           \
    memset(table->scale_array->flags, 0, sizeof(uint64_t) * (128 + 63) / 64); \
    table->scale_array->size = 0;                                             \
    table->iter.offset = 0;                                                   \
    table->iter.status = 0;                                                   \
    table->iter.m = 0;                                                        \
    table->iter.type = 0;                                                     \
    table->iter.size = 0;                                                     \
    return table;                                                             \
  }                                                                           \
  static inline hash##name##_t* hash##name##_init(size_t max_size, int m)     \
  {                                                                           \
    return hash##name##_init_with_load(max_size, m, HASH_LOAD_FACTOR);        \
  }                                                                           \
  static inline void hash##name##_free(hash##name##_t* table)                 \
  {                                                                           \
    for (int i = 0; i < table->m; i++) {                                      \
      hash_free(table->array[i].entries);                                     \
      hash_free(table->array[i].flags);                                       \
    }                                                                         \
    hash_free(table->array);                                                  \
    for (uint64_t i = 0; i < table->scale_array->capacity; i++) {             \
      if (__is_set(table->scale_array->flags, i)) {                           \
        hash_free(table->scale_array->entries[i]);                            \
      }                                                                       \
    }                                                                         \
    hash_free(table->scale_array->entries);                                   \
    hash_free(table->scale_array->flags);                                     \
    hash_free(table->scale_array);                                            \
    hash_free(table);                                                         \
  }                                                                           \
  static inline hash##name##_entry_t* hash##name##_get_with_from(             \
      hash##name##_t* table, ktype key, int* found)                           \
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
    if (table->scale_array->size == 0) {                                      \
      return NULL;                                                            \
    }                                                                         \
    uint64_t index = h % (table->scale_array->capacity - 1);                  \
    hash##name##_entry_t** entries = table->scale_array->entries;             \
    uint64_t i = index;                                                       \
    uint64_t lookup = 0;                                                      \
    uint64_t step = 0;                                                        \
    while (step <= table->scale_array->capacity) {                            \
      if (lookup >= table->scale_array->size) {                               \
        break;                                                                \
      }                                                                       \
      if (i >= table->scale_array->capacity) {                                \
        i = 0;                                                                \
      }                                                                       \
      if (__is_set(table->scale_array->flags, i)) {                           \
        if (feq(entries[i]->key, key)) {                                      \
          *found = 1;                                                         \
          return entries[i];                                                  \
        }                                                                     \
        lookup++;                                                             \
      }                                                                       \
      i++;                                                                    \
      step++;                                                                 \
    }                                                                         \
    return NULL;                                                              \
  }                                                                           \
  static inline hash##name##_entry_t* hash##name##_get(hash##name##_t* table, \
                                                       ktype key, int* found) \
  {                                                                           \
    return hash##name##_get_with_from(table, key, found);                     \
  }                                                                           \
  static inline hash##name##_entry_t* __hash##name##_put(                     \
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
    _hash##name##_scale_array_t* scale_array = table->scale_array;            \
    if ((double)scale_array->size / (double)scale_array->capacity             \
        > table->load) {                                                      \
      uint64_t old_cap = scale_array->capacity;                               \
      uint64_t new_cap = __hash_roundup64_dwp__(scale_array->capacity + 1);   \
      assert(new_cap > old_cap);                                              \
      uint64_t* flags = scale_array->flags;                                   \
      hash##name##_entry_t** entries = scale_array->entries;                  \
      scale_array->flags =                                                    \
          (uint64_t*)hash_malloc(sizeof(uint64_t) * (new_cap + 63) / 64);     \
      memset(scale_array->flags, 0, sizeof(uint64_t) * (new_cap + 63) / 64);  \
      scale_array->entries = (hash##name##_entry_t**)hash_malloc(             \
          sizeof(hash##name##_entry_t*) * new_cap);                           \
      memset(scale_array->entries, 0,                                         \
             sizeof(hash##name##_entry_t*) * new_cap);                        \
      for (uint64_t i = 0; i < old_cap; i++) {                                \
        if (!__is_set(flags, i)) {                                            \
          continue;                                                           \
        }                                                                     \
        uint64_t index = fhash(entries[i]->key) % (new_cap - 1);              \
        if (__is_set(scale_array->flags, index)) {                            \
          uint64_t j = index;                                                 \
          uint64_t start = index;                                             \
          while (1) {                                                         \
            if (j >= new_cap) {                                               \
              j = 0;                                                          \
            }                                                                 \
            if (!__is_set(scale_array->flags, j)) {                           \
              __set(scale_array->flags, j);                                   \
              scale_array->entries[j] = entries[i];                           \
              break;                                                          \
            }                                                                 \
            j++;                                                              \
            if (j == start) {                                                 \
              break;                                                          \
            }                                                                 \
          }                                                                   \
        } else {                                                              \
          __set(scale_array->flags, index);                                   \
          scale_array->entries[index] = entries[i];                           \
        }                                                                     \
      }                                                                       \
      hash_free(flags);                                                       \
      hash_free(entries);                                                     \
      scale_array->capacity = new_cap;                                        \
    }                                                                         \
    uint64_t index = h % (scale_array->capacity - 1);                         \
    {                                                                         \
      hash##name##_entry_t** entries = scale_array->entries;                  \
      uint64_t i = index;                                                     \
      while (1) {                                                             \
        if (i >= scale_array->capacity) {                                     \
          i = 0;                                                              \
        }                                                                     \
        if (__is_set(scale_array->flags, i)) {                                \
          if (feq(entries[i]->key, key)) {                                    \
            if (replace) {                                                    \
              hash##name##_entry_t* e = (hash##name##_entry_t*)hash_malloc(   \
                  sizeof(hash##name##_entry_t));                              \
              memcpy(e, entry, sizeof(hash##name##_entry_t));                 \
              entries[i] = e;                                                 \
            }                                                                 \
            *exist = 1;                                                       \
            return entries[i];                                                \
          }                                                                   \
          i++;                                                                \
          continue;                                                           \
        }                                                                     \
        __set(scale_array->flags, i);                                         \
        scale_array->size++;                                                  \
        table->size++;                                                        \
        hash##name##_entry_t* e =                                             \
            (hash##name##_entry_t*)hash_malloc(sizeof(hash##name##_entry_t)); \
        memcpy(e, entry, sizeof(hash##name##_entry_t));                       \
        entries[i] = e;                                                       \
        return entries[i];                                                    \
      }                                                                       \
    }                                                                         \
    return NULL;                                                              \
  }                                                                           \
  static inline hash##name##_entry_t* hash##name##_put(                       \
      hash##name##_t* table, hash##name##_entry_t* entry)                     \
  {                                                                           \
    int exist;                                                                \
    return __hash##name##_put(table, entry, 1, &exist);                       \
  }                                                                           \
  static inline hash##name##_entry_t* hash##name##_put_if_not(                \
      hash##name##_t* table, hash##name##_entry_t* entry, int replace,        \
      int* exist)                                                             \
  {                                                                           \
    return __hash##name##_put(table, entry, replace, exist);                  \
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
    if (!table->scale_array->size) {                                          \
      table->iter.status = 1;                                                 \
      return NULL;                                                            \
    }                                                                         \
    _hash##name##_scale_array_t* scale_array = table->scale_array;            \
    hash##name##_entry_t** entries = scale_array->entries;                    \
    while (!__is_set(scale_array->flags, table->iter.offset)                  \
           && table->iter.offset < scale_array->capacity) {                   \
      table->iter.offset++;                                                   \
    }                                                                         \
    if (table->iter.offset == scale_array->capacity) {                        \
      table->iter.status = 1;                                                 \
      return NULL;                                                            \
    }                                                                         \
    table->iter.size++;                                                       \
    return entries[table->iter.offset++];                                     \
  }

#define define_hashtable(name, ktype, vtype, feq, fhash)                      \
  __define_hash(table_##name, ktype, vtype, feq, fhash);                      \
  __define_hash_method(table_##name, feq, fhash, ktype, vtype);

// clang-format off
#define hashtable_init_with_load(name, max_size, m, load) hashtable_##name##_init_with_load(max_size, m, load)
#define hashtable_init(name, max_size, m)        hashtable_##name##_init(max_size, m)
#define hashtable_free(name, table)              hashtable_##name##_free(table)
#define hashtable_get(name, table, key, found)   hashtable_##name##_get(table, key, found)
#define hashtable_put(name, table, entry)        hashtable_##name##_put(table, entry)
#define hashtable_iter(name, table)              hashtable_##name##_iter(table)
// clang-format on

#define define_hashset(name, ketype, feq, fhash)                              \
  __define_hash(set_##name, ketype, NULL, feq, fhash);                        \
  __define_hash_method(set_##name, feq, fhash, ketype, NULL);

// clang-format off
#define hashset_init_with_load(name, max_size, m, load) hashset_##name##_init_with_load(max_size, m, load)
#define hashset_init(name, max_size, m)          hashset_##name##_init(max_size, m)
#define hashset_free(name, table)                hashset_##name##_free(table)
#define hashset_get(name, table, key, found)     hashset_##name##_get(table, key, found)
#define hashset_put(name, table, entry)          hashset_##name##_put(table, entry)
#define hashset_iter(name, table)                hashset_##name##_iter(table)
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

define_hashtable(ii, int, int, __hash_eq_number, __hash_hash_u32);
define_hashtable(ll, int64_t, int64_t, __hash_eq_number, __hash_hash_u64);
define_hashtable(si, char*, int, __hash_eq_string, __hash_hash_string);
define_hashtable(ss, char*, char*, __hash_eq_string, __hash_hash_string);
define_hashtable(li, int64_t, int, __hash_eq_number, __hash_hash_u64);

define_hashset(i, int, __hash_eq_number, __hash_hash_u32);
define_hashset(l, int64_t, __hash_eq_number, __hash_hash_u64);
define_hashset(s, char*, __hash_eq_string, __hash_hash_string);

#ifdef __cplusplus
}
#endif
#endif
