#pragma once

#define STRINGIFY(x) #x

DECL_VECTOR(int, int);
DECL_VECTOR(char*, char_ptr);

int test_strings();
int test_ints();

// Prints int vector
void print_int_vector(Vector(int)* v);

// Prints char* vector
void print_cstr_vector(Vector(char_ptr)* v);
