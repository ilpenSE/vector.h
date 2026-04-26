# `vector.h` Library - `std::vector` in C

- This library is my implementation of vectors. (AKA dynamic arrays)
- It's meant to be used only in C. (>=C11)
- It has GNU extensions (like expression-statements) so be careful if you don't use GCC or Clang.
- It is type-generic library and it uses macros with functions. Macros wrap functions.
- Everything starts with `vec_*` is wrapper macro.
- Everything starts with `_vec_*` is internal or generic function.
- Everything starts with `VEC_*` is internal macro.
  (But you can change `VEC_INITIAL_CAPACITY` in header.)

## Usage
```c
DECL_VECTOR(int, int);
```
- The first argument is T which is C type of an item.
- The second argument is TName which is name of the type that you want to pass to Vector struct's name.
  (pointers should not be passed like `int*`
   but you can do `typedef int* int_ptr` then use `DECL_VECTOR(int*, int_ptr)`)

Then use it like this:
```c
Vector(int) vec = {0};
Vector(int) func();
void some_func(Vector(int) v1, Vector(int) v2);
```
- Since `DECL_VECTOR` macro declares a struct, v1 and v2 has compatible types and you can pass it through function parameters.

## Functions/Macros

- NOTE: All functions/macros expect NON-NULL vector pointer (context) which is the first argument
  -> So you cannot have `vec_push(NULL, 10);` this is segfault.
  -> But you can have `vec_push(&vec, NULL);` this append NULL to the vec.

### `bool vec_push(Vector(T)* vec, T item)`:
    - `item` parameter can be rvalue or lvalue it doesn't matter.
    - Pushes new element to the vector.
    - It can invalidate pointer aliases to original vector pointer because it calls `realloc`.

### `bool vec_push_many(Vector(T)* vec, T items...)`:
    - `items` parameter is variadic. It'll generate intermediate array and pass it to the internal.
    - Pushes new elements to the vector.
    - It can invalidate pointer aliases to original vector pointer because it calls `realloc`.

### `void vec_free(Vector(T)* vec)`
    - Frees the vector from heap and sets `is_freed` to true.
    - So `vec_isfreed()` can catch Use-After-Frees
    - It sets len and cap fields to zero, index operations are automatically return false because len is zero.

### `bool vec_isfreed(Vector(T)* vec)`
    - Detects if vec is freed or not.
    - Returns `is_freed` field from header.
    - Doesn't check capacity or any field.

### `bool vec_pop(Vector(T)* vec, void* out)`
    - Removes last element (pops back) from vec.
    - If len is zero, returns false.
    - You can catch that element into out variable.
    - If out is NULL, it doesn't try to copy last element into it and it's simply forgotten.
    - It performs O(1) operation.

### `bool vec_remove_unord(Vector(T)* vec, size_t idx)`
    - Removes an element with any index
    - It copies last element's value into given index.
    - The element will be forgotten, you cannot catch it. (Save before calling it if you want)
      -> This may cause memory leak if you have vector of a vector.
    - It will change the order of elements.
    - Checks out-of-bounds. If so, returns false.
    - It performs O(1) operation.

### `bool vec_remove_idx(Vector(T)* vec, size_t idx)`
    - Removes an element with any index.
    - It'll shift the array after the element. So it overwrites the element.
    - Again, like in `vec_remove_unord` the element'll be forgotten.
    - This doesn't change order of the elements but it copies subset of the array,
      worst-case is O(n) which is removing first element.
    - Checks out-of-bounds. If so, returns false.

### `int vec_find(Vector(T)* vec, T item)`
    - It tries to find that item in the array using memcmp everytime.
    - If it cannot find, returns -1.

### `T* vec_at(Vector(T)* vec, size_t idx)`
    - Returns element that has index `idx`.
    - It acts like `vec[idx]` (since you cannot use `[]` when accessing an element, use this instead)
    - Checks boundaries. Returns NULL if idx violates boundaries of vector or vector might be freed.

### `T* vec_first(Vector(T)* vec)` and `T* vec_last(Vector(T)* vec)`
    - They're accessing first and last element of vector.
    - Wrapper of vec_at, simply calls `vec_at(0)` for first, `vec_at(vec_len(v) - 1)` for last.

### `bool vec_isfirst(Vector(T)* vec, T item)` and `bool vec_islast(Vector(T)* vec, T item)`
    - Checks if given item is last or first element of given vector.
    - If there's no last or first element (maybe vector was freed or empty) they return false.

### `size_t vec_cap(Vector(T)* vec)`
    - Returns capacity of the vector.

### `size_t vec_len(Vector(T)* vec)`
    - Returns length of the vector.

### `size_t vec_esize(Vector(T)* vec)`
    - Returns size of an element in the vector.

### `vec_foreach(Vector(T)* vec, it)`
    - Organizes foreach loop for given vector.
    - `it` is the variable name of the iterator. And it's pointer.
    - You can use this like: `vec_foreach(&vec, it) printf("%d\n", *it);`
      (if vec is vector of integers)
    - If vector is freed, it aborts the program and fails with assertion failure

### `bool vec_equals(Vector(T)* lhs, Vector(T)* rhs)`
    - Compares 2 vectors if they're equal
    - If their lengths and items within the length same, it returns true
    - Firstly compares items pointers then lengths and finally the whole items buffer with memcmp

### `bool vec_reserve(Vector(T)* vec, size_t extra)`
    - Reverses extra amount of spaces into vec.
    - It may fail (realloc fail or size_t overflows)

### `bool vec_contains(Vector(T)* vec, T* item)`
    - Tries to find that item with calling `vec_at`
    - If `vec_at` returns -1, this'll return false.

# About

- Made by [@ilpenSE](https://github.com/ilpenSE/)

