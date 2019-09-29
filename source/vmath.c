#include <stdio.h>
#include <math.h>

#include "vmath.h"

vec_t v3neg(vec_t v)
{
  vec_t result = {.data = {
    -v.data[0],
    -v.data[1],
    -v.data[2],
    v.data[3]
  }};
  return result;
}

vec_t v3av3(vec_t a, vec_t b)
{
  vec_t result = {.data = {
    a.data[0] + b.data[0],
    a.data[1] + b.data[1],
    a.data[2] + b.data[2],
    a.data[3]// + b.data[3]
  }};
  return result;
}

vec_t v3sv3(vec_t a, vec_t b)
{
  vec_t result = {.data = {
    a.data[0] - b.data[0],
    a.data[1] - b.data[1],
    a.data[2] - b.data[2],
    a.data[3]// - b.data[3]
  }};
  return result;
}

float v3dv3(vec_t a, vec_t b)
{
  return a.data[0]*b.data[0] + a.data[1]*b.data[1] + a.data[2]*b.data[2];
}

vec_t v3xv3(vec_t a, vec_t b)
{
  vec_t result ={
    .data = {
      a.data[1]*b.data[2] - a.data[2]*b.data[1],
      a.data[2]*b.data[0] - a.data[0]*b.data[2], // WAS 3
      a.data[0]*b.data[1] - a.data[1]*b.data[0]
    }
  };
  return result;
}

mat_t m4xm4(mat_t a, mat_t b)
{
  mat_t result = {.data = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}};
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      for(int k = 0; k < 4; k++){
        result.data[i][j] += a.data[i][k]*b.data[k][j];
      }
    }
  }
  return result;
}

vec_t m4xv4(mat_t a, vec_t b)
{
  vec_t result = {.data={b.data[0],b.data[2],b.data[3],b.data[3]}};
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      result.data[i] += a.data[i][j]*b.data[j];
    }
  }
  return result;
}

mat_t translate(vec_t v)
{
  mat_t res = {.data = {
    {1, 0, 0, v.data[0]},
    {0, 1, 0, v.data[1]},
    {0, 0, 1, v.data[2]},
    {0, 0, 0, 1}
  }};
  return res;
}

mat_t frustum(float l, float r, float t, float b, float n, float f)
{
  mat_t res = {.data = {
    {2*n/(r-l), 0,         (r+l)/(r-l),  0},
    {0,         2*n/(t-b), (t+b)/(t-b),  0},
    {0,         0,        -(f+n)/(f-n), -2*f*n/(f-n)},
    {0,         0,        -1,            0}
  }};
  return res;
}

mat_t perspective(float fovy, float aspect, float n, float f)
{
  float cot = cos(fovy/2.f) / sin(fovy/2.f);
  mat_t res = {.data = {
    {cot/aspect, 0, 0, 0},
    {0, cot, 0, 0},
    {0, 0, (f+n)/(n-f), 2*f*n/(n-f)},
    {0, 0, -1, 0}
  }};
  return res;
}

mat_t rotate(vec_t axis, float angle)
{
  float x = axis.data[0], y = axis.data[1], z = axis.data[2];
  float c = cosf(angle), s = sinf(angle);
  mat_t rot = {.data = {
    {x*x*(1-c)+c, x*y*(1-c)-z*s, x*z*(1-c)+y*s, 0},
    {y*x*(1-c)+z*s, y*y*(1-c)+c, y*z*(1-c)-x*s, 0},
    {x*z*(1-c)-y*s, y*z*(1-c)+x*s, z*z*(1-c)+c, 0},
    {0, 0, 0, 1}
  }};
  return rot;
}

void print_vec(vec_t v)
{
  printf(
    "%f, %f, %f, %f\n",
    v.data[0], v.data[1], v.data[2], v.data[3]
  );
}

void print_mat(mat_t m)
{
  printf(
    "%f, %f, %f, %f\n"
    "%f, %f, %f, %f\n"
    "%f, %f, %f, %f\n"
    "%f, %f, %f, %f\n",
    m.data[0][0], m.data[0][1], m.data[0][2], m.data[0][3],
    m.data[1][0], m.data[1][1], m.data[1][2], m.data[1][3],
    m.data[2][0], m.data[2][1], m.data[2][2], m.data[2][3],
    m.data[3][0], m.data[3][1], m.data[3][2], m.data[3][3]
  );
}
