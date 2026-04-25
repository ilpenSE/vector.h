#include <stdio.h>
#include <vector.h>
#include "utils.h"

int test_ints() {
  Vector(int) vec = {0};
  vec_push(&vec, 10);
  vec_push(&vec, 20);
  vec_push(&vec, 30);
  vec_push(&vec, 40);
  print_int_vector(&vec);

  vec_push_many(&vec, 69, 67);
  print_int_vector(&vec);

  vec_pop(&vec, NULL);
  print_int_vector(&vec);

  printf("elem in idx 1: %d\n", vec_at(&vec, 1));
  printf("last elem: %d\n", vec_last(&vec));

  printf("foreach: [");
  vec_foreach(&vec, it) {
    printf("%d, ", *it);
  }
  printf("]\n");

  printf(vec_islast(&vec, 69) ? "69 IS LAST!\n" : "69 no last\n");
  printf(vec_isfirst(&vec, 10) ? "10 IS FIRST!\n" : "10 no first\n");

  printf("vec_idx: %d\n", vec_idx(&vec, 20));
  printf(vec_contains(&vec, 620) ? "contains\n" : "doesnt contain\n");

  vec_remove_unord(&vec, 1); // remove elem with idx 1 (20 in our case)
  printf("Removed 20\n");
  print_int_vector(&vec);

  vec_free(&vec);
  print_int_vector(&vec);
  return 0;
}
