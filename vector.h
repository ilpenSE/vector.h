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

  int x = vec_at(&v, 0);
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

typedef struct {
  VectorHeader* header;
  void** items;
  size_t elem_size;
} VectorGeneric;

#define VEC_TO_GENERIC(v) ((VectorGeneric){&(v)->h, (void**)&(v)->items, sizeof((v)->_type_tag)})

// User-Space macros, you want to use them:

#define vec_init(TName) (Vector(TName)){0}

#define vec_push(v, val)                                            \
  __extension__({                                                   \
      __typeof__((v)->_type_tag) _tmp = (val);                      \
      _vec_push(VEC_TO_GENERIC((v)), &_tmp); \
    })

#define vec_push_many(v, ...)                                           \
  __extension__({                                                       \
      __typeof__((v)->_type_tag) _arr[] = {__VA_ARGS__};                \
      _vec_push_many(VEC_TO_GENERIC((v)), _arr, sizeof(_arr)/sizeof(_arr[0])); \
    })

#define vec_remove_unord(v, idx) \
  _vec_remove_unord(VEC_TO_GENERIC((v)), (idx))

#define vec_at(v, index)                                             \
  (*((__typeof__((v)->_type_tag)*)                                    \
     _vec_at(VEC_TO_GENERIC((v)), (index))))

#define vec_idx(v, item)                                             \
  __extension__({                                                   \
      __typeof__((v)->_type_tag) _tmp = (item);                      \
      _vec_idx(VEC_TO_GENERIC((v)), &_tmp); \
    })

#define vec_contains(v, item) (vec_idx(v, item) != -1)

#define vec_pop(v, out)                                               \
  _vec_pop(VEC_TO_GENERIC((v)), (out))

#define vec_reserve(v, extra)                                           \
  _vec_reserve(VEC_TO_GENERIC((v)), (extra))

#define vec_free(v) _vec_free(VEC_TO_GENERIC((v)))

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

#define vec_last(v) (vec_at((v), vec_len((v)) - 1))
#define vec_first(v) (vec_at((v), 0))

#define vec_islast(v, item) (vec_last((v)) == item)
#define vec_isfirst(v, item) (vec_first((v)) == item)

#define vec_foreach(v, it) for(__typeof__((v)->_type_tag)* it = (v)->items; it < (v)->items + vec_len((v)); it++)

// Raw Functions

VECTORDEF bool _vec_push(VectorGeneric v, const void* value);

VECTORDEF bool _vec_push_many(VectorGeneric v, const void* values, size_t count);

VECTORDEF bool _vec_remove_unord(VectorGeneric v, size_t idx);

VECTORDEF void* _vec_at(VectorGeneric v, size_t index);

VECTORDEF int _vec_idx(VectorGeneric v, const void* item);

VECTORDEF bool _vec_pop(VectorGeneric v, void* out);

VECTORDEF bool _vec_reserve(VectorGeneric v, size_t extra);

VECTORDEF void _vec_free(VectorGeneric v);

VECTORDEF bool _vec_equals(VectorHeader* lh, void* li,
                           VectorHeader* rh, void* ri,
                           size_t les, size_t res);

#ifdef VECTOR_IMPLEMENTATION

void _vec_free(VectorGeneric v) {
  free(*v.items);
  *v.items = NULL;
  v.header->len = 0;
  v.header->cap = 0;
}

bool _vec_reserve(VectorGeneric v, size_t extra) {
  if (v.header->cap >= SIZE_MAX / 2) return false;
  size_t needed = v.header->len + extra;

  // enough capacity, no need to realloc
  if (v.header->cap >= needed) return true;

  // calculate new capacity
  size_t new_cap = v.header->cap ? v.header->cap : VEC_INITIAL_CAPACITY;
  while (new_cap < needed)
    new_cap *= 2;

  void* tmp = realloc(*v.items, new_cap * v.elem_size);
  if (!tmp) return false;
  *v.items = tmp;
  v.header->cap = new_cap;
  return true;
}

bool _vec_pop(VectorGeneric v, void* out) {
  if (v.header->len == 0) return false;
  if (out) memcpy(out, (char*)(*v.items) + (v.header->len - 1) * v.elem_size, v.elem_size);
  v.header->len -= 1;
  return true;
}

bool _vec_push(VectorGeneric v, const void* value) {
  if (!_vec_reserve(v, 1)) return false;

  memcpy(
    (char*)(*v.items) + v.header->len * v.elem_size,
    value,
    v.elem_size
    );
  v.header->len += 1;
  return true;
}

bool _vec_push_many(VectorGeneric v, const void* values, size_t count) {
  if (!_vec_reserve(v, count)) return false;

  memcpy(
    (char*)(*v.items) + v.header->len * v.elem_size,
    values,
    v.elem_size * count
    );
  v.header->len += count;
  return true;
}

bool _vec_remove_unord(VectorGeneric v, size_t idx) {
  if (idx >= v.header->len) return false;
  memmove(_vec_at(v, idx), _vec_at(v, v.header->len - 1), v.elem_size);
  v.header->len -= 1;
  return true;
}

void* _vec_at(VectorGeneric v, size_t idx) {
  if (!(*v.items) || idx >= v.header->len) return NULL;
  return (char*)(*v.items) + idx * v.elem_size;
}

int _vec_idx(VectorGeneric v, const void* item) {
  for (size_t i = 0; i < v.header->len; i++) {
    uint8_t* b = (uint8_t*)(*v.items) + i * v.elem_size;
    vec_item_comparator_t comp_fn = v.header->item_comparator ? v.header->item_comparator : memcmp;
    if (comp_fn(b, item, v.elem_size) == 0) return i;
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
