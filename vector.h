#ifndef VECTOR_H
#define VECTOR_H

/*
  STB-Style single-header vector.h library
  Define VECTOR_IMPLEMENTATION in one translation unit before including.

  Usage:
  #define VECTOR_IMPLEMENTATION
  #include “vector.h”

  typedef char* char_ptr;
  DECL_VECTOR(int, int)
  DECL_VECTOR(float, float)
  DECL_VECTOR(char*, char_ptr)

  int main(void) {
  Vector(int) v = {0};

  vec_push(&v, 42);
  vec_push(&v, 99);

  int x = vec_get(&v, 0);
  int len = vec_len(&v);

  vec_free(&v);
  }

*/

#ifdef __cplusplus
  #define VECTORDEF extern “C”
#else
  #define VECTORDEF extern
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define VEC_INITIAL_CAPACITY 64
typedef char* char_ptr;
typedef int* int_ptr;

typedef int (*vec_item_comparator_t)(const void* lhs, const void* rhs, size_t elem_size);

typedef struct {
  vec_item_comparator_t item_comparator;
  size_t cap;
  size_t len;
} VectorHeader;

#define Vector(TName) Vector_##TName

#define DECL_VECTOR(T, TName)                   \
  typedef struct {                              \
    VectorHeader h;                             \
    T* items;                                   \
    T _type_tag;                                \
  } Vector_##TName;

// User-Space macros, you want to use them:

#define vec_init(TName) (Vector(TName)){0}

#define vec_push(v, val)                                            \
  __extension__({                                                   \
      __typeof__((v)->_type_tag) _tmp = (val);                      \
      _vec_push(&(v)->h, (void**)&(v)->items, sizeof(_tmp), &_tmp); \
    })

#define vec_push_many(v, ...)                                           \
  __extension__({                                                       \
      __typeof__((v)->_type_tag) _arr[] = {__VA_ARGS__};                \
      _vec_push_many(&(v)->h, (void**)&(v)->items,                      \
                     sizeof((v)->_type_tag), _arr, sizeof(_arr)/sizeof(_arr[0])); \
    })

#define vec_get(v, index)                                             \
  (*((__typeof__((v)->_type_tag)*)                                    \
     _vec_get(&(v)->h, (v)->items, sizeof((v)->_type_tag), (index))))

#define vec_idx(v, item)                                             \
  __extension__({                                                   \
      __typeof__((v)->_type_tag) _tmp = (item);                      \
      _vec_idx(&(v)->h, (void**)&(v)->items, sizeof(_tmp), &_tmp); \
    })

#define vec_contains(v, item) (vec_idx(v, item) != -1)

#define vec_pop(v, out)                                               \
  _vec_pop(&(v)->h, (void**)&(v)->items, sizeof((v)->_type_tag), (out))

#define vec_reserve(v, extra)                                           \
  _vec_reserve(&(v)->h, (void**)&(v)->items, sizeof(((v)->_type_tag)), (extra))

#define vec_free(v) _vec_free(&(v)->h, (void**)&(v)->items)

#define vec_override_item_comparator(v, func) \
  ((v)->h.item_comparator = func)

#define vec_equals(lv, rv)                      \
  _vec_equals(&(lv)->h, (lv)->items,            \
              &(rv)->h, (rv)->items,            \
              sizeof((lv)->_type_tag),          \
              sizeof((rv)->_type_tag))

// Convenience accessors
#define vec_len(v)   ((v)->h.len)
#define vec_cap(v)   ((v)->h.cap)
#define vec_esize(v) (sizeof((v)->_type_tag))

#define vec_last(v) (vec_get((v), vec_len((v)) - 1))
#define vec_first(v) (vec_get((v), 0))

#define vec_islast(v, item) (vec_last((v)) == item)
#define vec_isfirst(v, item) (vec_first((v)) == item)

#define vec_foreach(v, it) for(__typeof__((v)->_type_tag)* it = (v)->items; it < (v)->items + vec_len((v)); it++)

// Raw Functions

VECTORDEF bool _vec_push(VectorHeader* h, void** items,
                         size_t elem_size, const void* value);

VECTORDEF bool _vec_push_many(VectorHeader* h, void** items,
                               size_t elem_size, const void* values, size_t count);

VECTORDEF void* _vec_get(VectorHeader* h, void* items,
                        size_t elem_size, size_t index);

VECTORDEF int _vec_idx(VectorHeader* h, void** items,
                       size_t elem_size, const void* item);

VECTORDEF bool _vec_pop(VectorHeader* h, void** items,
                        size_t elem_size, void* out);

VECTORDEF bool _vec_reserve(VectorHeader* h, void** items,
                            size_t elem_size, size_t extra);

VECTORDEF void _vec_free(VectorHeader* h, void** items);

VECTORDEF bool _vec_equals(VectorHeader* lh, void* li,
                           VectorHeader* rh, void* ri,
                           size_t les, size_t res);

#ifdef VECTOR_IMPLEMENTATION

void _vec_free(VectorHeader* h, void** items) {
  free(*items);
  *items = NULL;
  h->len = 0;
  h->cap = 0;
}

bool _vec_reserve(VectorHeader* h, void** items, size_t elem_size, size_t extra) {
  if (h->cap >= SIZE_MAX / 2) return false;
  size_t needed = h->len + extra;

  // enough capacity, no need to realloc
  if (h->cap >= needed) return true;

  // calculate new capacity
  size_t new_cap = h->cap ? h->cap : VEC_INITIAL_CAPACITY;
  while (new_cap < needed)
    new_cap *= 2;

  void* tmp = realloc(*items, new_cap * elem_size);
  if (!tmp) return false;
  *items = tmp;
  h->cap = new_cap;
  return true;
}

bool _vec_pop(VectorHeader* h, void** items, size_t elem_size, void* out) {
  if (h->len == 0) return false;
  if (out) memcpy(out, (char*)(*items) + (h->len - 1) * elem_size, elem_size);
  h->len -= 1;
  return true;
}

bool _vec_push(VectorHeader* h, void** items, size_t elem_size, const void* value) {
  if (!_vec_reserve(h, items, elem_size, 1)) return false;

  memcpy(
    (char*)(*items) + h->len * elem_size,
    value,
    elem_size
    );
  h->len += 1;
  return true;
}

bool _vec_push_many(VectorHeader* h, void** items, size_t elem_size, const void* values, size_t count) {
  if (!_vec_reserve(h, items, elem_size, count)) return false;

  memcpy(
    (char*)(*items) + h->len * elem_size,
    values,
    elem_size * count
    );
  h->len += count;
  return true;
}

void* _vec_get(VectorHeader* h, void* items, size_t elem_size, size_t index) {
  if (!items || index >= h->len) return NULL;
  return (char*)items + index * elem_size;
}

int _vec_idx(VectorHeader* h, void** items, size_t elem_size, const void* item) {
  for (size_t i = 0; i < h->len; i++) {
    uint8_t* b = (uint8_t*)(*items) + i * elem_size;
    vec_item_comparator_t comp_fn = h->item_comparator ? h->item_comparator : memcmp;
    if (comp_fn(b, item, elem_size) == 0) return i;
  }
  return -1;
}

// lh/rh -> left/right-hand side header
// li/ri -> left/right-hand side items
// les/res -> left/right-hand side element size (elem_size)
bool _vec_equals(VectorHeader* lh, void* li, VectorHeader* rh, void* ri,
                 size_t les, size_t res) {
  if (li == ri) return true;
  if (les != res || lh->len != rh->len) return false;
  return memcmp(li, ri, lh->len * les) == 0;
}

#endif // VECTOR_IMPLEMENTATION
#endif // VECTOR_H
