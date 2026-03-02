#include <stdio.h>
#define VECTOR_IMPLEMENTATION
#include "vector.h"

// Prints int vector
void print_vector(Vector* v);
bool testInit(Vector* given);
bool testPushVal(Vector* given);
bool testPush(Vector* given);
bool testPushMany(Vector* given);
bool testUAF(Vector* given);

int main() {
  size_t szarr[] = {1, 2, 3, 4, 5};
  Vector vec = (Vector) {
    .items = szarr,
    .elem_size = sizeof(size_t),
    .cap = 8,
    .len = 5,
  };
  print_vector(&vec);
  printf("\n");

  Vector v;
  if (!vec_init(&v, sizeof(int))) return 1;
  if (!testInit(&v)) printf("Init test failed!\n");
  else print_vector(&v);
  printf("\n");

  // pushing literals
  vec_push_val(&v, 4);
  if (!testPushVal(&v)) printf("Push val test failed!\n");
  else print_vector(&v);
  printf("\n");

  // pushing variables
  int x = 5;
  vec_push(&v, &x);
  if (!testPush(&v)) printf("Push test failed!\n");
  else print_vector(&v);
  printf("\n");

  int arr[3] = {1, 2, 3};
  vec_push_many(&v, arr, 3);
  if (!testPushMany(&v)) printf("Push many test failed!\n");
  else print_vector(&v);
  printf("\n");

  int a = *(int*)vec_get(&v, 0);
  int b = vec_get_as(&v, 0, int);
  if (a != b) printf("Get test failed!\n");
  else {
    printf("0th elem: %d\n", a);
    printf("0th elem: %d\n", b);
  }
  printf("\n");

  // UAF (Use-After-Free) test
  if (!testUAF(&v)) printf("Use-After-Free test failed!\n");
  else printf("Vector capacity after free: %zu\n", v.cap);
  return 0;
}
