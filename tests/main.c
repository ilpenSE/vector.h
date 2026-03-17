#include <stdio.h>
#include <string.h>
#define VECTOR_IMPLEMENTATION
#include <vector.h>
#include "utils.h"

int main(int argc, char** argv) {
  const char* kind = "";
  if (argc >= 2) kind = argv[1];

  if (strcmp(kind, "ints") == 0) return test_ints();
  if (strcmp(kind, "strings") == 0) return test_strings();

  printf("Unknown test kind.\n");
  printf("Usage:\n");
  printf("  %s ints   : Test int vectors\n", argv[0]);
  printf("  %s strings: Test string (char*) vectors\n", argv[0]);
  return 1;
}

void print_int_vector(Vector(int)* v) {
  size_t len = vec_len(v);
  printf(STRINGIFY(Vector(int)) "{items: [");
  if (v->items) {
    for (size_t i = 0; i < len; i++) {
      int elem = vec_get(v, i);
      printf("%d%s", elem, (i == len - 1 ? "" : ", "));
    }
  }
  printf("], ");
  printf("capacity: %zu, ", vec_cap(v));
  printf("length: %zu}\n", len);
}

void print_cstr_vector(Vector(char_ptr)* v) {
  size_t len = vec_len(v);
  printf(STRINGIFY(Vector(char_ptr)) "{items: [");
  if (v->items) {
    for (size_t i = 0; i < len; i++) {
      char* elem = vec_get(v, i);
      printf("\"%s\"%s", elem, (i == len - 1 ? "" : ", "));
    }
  }
  printf("], ");
  printf("capacity: %zu, ", vec_cap(v));
  printf("length: %zu}\n", len);
}
