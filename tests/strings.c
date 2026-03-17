#include <stdio.h>
#include <string.h>
#include <vector.h>
#include "utils.h"

static int str_vec_cmp(const void* a, const void* b, size_t sz) {
  (void)sz;
  return strcmp(*(const char**)a, *(const char**)b);
}

int test_strings() {
  Vector(char_ptr) vec = {0};
  vec_override_item_comparator(&vec, str_vec_cmp);
  vec_push(&vec, "hello");
  vec_push(&vec, "world");
  print_cstr_vector(&vec);
  printf("idx of hello: %d\n", vec_idx(&vec, "world"));

  vec_push_many(&vec, "selamın", "aleyküm", "agalar");
  print_cstr_vector(&vec);
  printf("idx of aleyküm: %d\n", vec_idx(&vec, "aleyküm"));
  return 0;
}
