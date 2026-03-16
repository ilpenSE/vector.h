#ifndef VECTOR_H
#define VECTOR_H

/*
  This is STB-Style singleheader vector.h library
  To use this, once define VECTOR_IMPLEMENTATION anywhere
  and then use it. Also its safe to use in C++ (no mangle)

  Vector API for C/C++ (C++ has std::vector, this is its implementation in C)
  It basically dynamic arrays, you declare one with DECL_VECTOR(Type, TypeName)
  and use like Vector(TypeName). We have lifetime manager functions and
  manipulator functions (push, pop) in here.
*/

#ifdef __cplusplus
  #define VECTORDEF extern "C"
#else
  #define VECTORDEF extern
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Type-Generic Vector struct
typedef struct {
  void* items; // items stored
  size_t elem_size; // size of 1 element (if you use int, provide sizeof(int))
  size_t cap; // capacity of vector in memory
  size_t len; // length of vector (count of elements)
} Vector;

/*
  Initializes a vector by size. The size depends on
  the type you put. If you use int, provide sizeof(int)
*/
VECTORDEF bool vec_init(Vector* v, size_t elem_size);

/*
  Frees the vector items and set capacity to zero
*/
VECTORDEF bool vec_free(Vector *v);

/*
  Pushes an element to the Vector named v, acts like da_append
  Value is void*, you have to cast it to void*,
  if you dont want any intermediate value, use vec_push_val
  you have to use this like this: (suppose you have vector of ints)
  
  int x = 20;
  vec_push(&v, &x);
  NOT THIS (SEGFAULT): vec_push(&v (void*)20);
*/
VECTORDEF bool vec_push(Vector* v, const void* value);

/*
  Pushes an array to the vector,
  count is how many elements in values
*/
VECTORDEF bool vec_push_many(Vector* v, const void* values, size_t count);

/*
  Reserves extra amount of elements.
  Extra is needed elements. NOT BYTES!
*/
VECTORDEF bool vec_reserve(Vector* v, size_t extra);

/*
  Gets an element by index. Returns pointer, so
  you have to do the cast work.
  If you dont want, use vec_get_as macro
*/
VECTORDEF void* vec_get(Vector* v, size_t index);

/*
  Compares two vectors and checks if they both are the same
*/
VECTORDEF bool vec_equals(Vector* lv, Vector* rv);

/*
  Pushes the literal values to vector, creates
  temporary variable and calls vec_push
*/
#define vec_push_val(v, val)                    \
  do {                                          \
    __typeof__(val) tmp = (val);                \
    vec_push((v), &tmp);                        \
  } while (0)

// vec_get returns pointer, this macro returns typeof the item
#define vec_get_as(v, index, T) (*(T*)vec_get((v), (index)))

#define VEC_INITIAL_CAPACITY 64

// IMPLEMENTATION
#ifdef VECTOR_IMPLEMENTATION

bool vec_init(Vector* v, size_t elem_size) {
  *v = (Vector) {0};
  v->elem_size = elem_size;
  if (!vec_reserve(v, VEC_INITIAL_CAPACITY)) return false;
  return true;
}

bool vec_free(Vector *v) {
  free(v->items);
  v->cap = 0;
  return true;
}

bool vec_reserve(Vector* v, size_t extra) {
  size_t needed = v->len + extra;

  // enough capacity, no need to realloc
  if (v->cap >= needed) return true;

  // for size_t overflows
  if (v->cap >= SIZE_MAX / 2) return false;

  // calculate new capacity
  size_t new_cap = v->cap ? v->cap : VEC_INITIAL_CAPACITY;
  while (new_cap < needed)
    new_cap *= 2; // exponential growth

  void* tmp = realloc(v->items, new_cap * v->elem_size);
  if (!tmp) return false;
  v->items = tmp;
  v->cap = new_cap;
  return true;
}

bool vec_push(Vector* v, const void* value) {
  if (!v->items) return false;
  if (!vec_reserve(v, 1)) return false;

  // cast v->items to char* to get raw byte offsets
  memcpy(
    (char*)v->items + v->len * v->elem_size,
    value,
    v->elem_size
    );
  v->len += 1;
  return true;
}

bool vec_push_many(Vector* v, const void* values, size_t count) {
  if (!v->items) return false;
  if (!vec_reserve(v, count)) return false;

  memcpy(
    (char*)v->items + v->len * v->elem_size,
    values,
    v->elem_size * count
    );
  v->len += count;
  return true;
}

void* vec_get(Vector* v, size_t index) {
  if (!v->items || index >= v->len) return NULL;
  return (char*)v->items + index * v->elem_size;
}

bool vec_equals(Vector* lv, Vector* rv) {
  if (lv == rv) return true;
  if (lv->elem_size != rv->elem_size) return false;
  if (lv->len != rv->len) return false;
  return memcmp(lv->items, rv->items, lv->len * lv->elem_size) == 0;
}

#endif // VECTOR_IMPLEMENTATION

#endif // VECTOR_H
