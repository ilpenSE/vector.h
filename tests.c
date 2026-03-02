#include "vector.h"
#include <stdio.h>
#include <stdbool.h>

// Prints int vector
void print_vector(Vector* v) {
  size_t len = v->len;
  printf("Vector items = [");
  if (v->items) {
    for (size_t i = 0; i < len; i++) {
      int elem = vec_get_as(v, i, int);
      printf("%d%s", elem, (i == len - 1 ? "" : ", "));
    }
  }
  printf("]\n");
  printf("Vector capacity: %zu\n", v->cap);
  printf("Vector length: %zu\n", v->len);
}

bool testInit(Vector* given) {
  if (!given) return false;
  Vector v = (Vector) {
    .items = (void*) {0},
    .elem_size = sizeof(int),
    .cap = 8,
    .len = 0,
  };
  return vec_equals(given, &v);
}

bool testPushVal(Vector* given) {
  if (!given) return false;
  int items[] = {4};
  Vector v = (Vector) {
    .items = items,
    .elem_size = sizeof(int),
    .cap = 8,
    .len = 1,
  };
  return vec_equals(given, &v);
}

bool testPush(Vector* given) {
  if (!given) return false;
  int items[] = {4, 5};
  Vector v = (Vector) {
    .items = items,
    .elem_size = sizeof(int),
    .cap = 8,
    .len = 2,
  };
  return vec_equals(given, &v);
}

bool testPushMany(Vector* given) {
  if (!given) return false;
  int items[] = {4, 5, 1, 2, 3};
  Vector v = (Vector) {
    .items = items,
    .elem_size = sizeof(int),
    .cap = 8,
    .len = 5,
  };
  return vec_equals(given, &v);
}

bool testUAF(Vector* given) {
  vec_free(given);
  for (size_t i = 0; i < given->len; i++) {
    // UB (Undefined behavior)
    // should have print garbage values or recieve SIGSEGV
    printf("elem %zu: %d\n", i, vec_get_as(given, i, int));
  }
  return true;
}
