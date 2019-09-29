#include <stdalign.h>

#ifndef VMATH_H
#define VMATH_H

typedef struct {
  alignas(16) float data[4];
} vec_t;

typedef struct {
  alignas(16) float data[4][4];
} mat_t;

vec_t v3neg(vec_t v);
vec_t v3av3(vec_t a, vec_t b);
vec_t v3sv3(vec_t a, vec_t b);
float v3dv3(vec_t a, vec_t b);
vec_t v3xv3(vec_t a, vec_t b);
mat_t m4xm4(mat_t a, mat_t b);
vec_t m4xv4(mat_t a, vec_t b);

void print_vec(vec_t v);
void print_mat(mat_t m);

//Useful Matrices
mat_t translate(vec_t v);
mat_t perspective(float fovy, float aspect, float n, float f);
mat_t frustum(float l, float r, float t, float b, float n, float f);
mat_t rotate(vec_t axis, float angle);

#endif

