#ifndef WAVEFRONT_H
#define WAVEFRONT_H

typedef union {
  struct {
    float x, y, z;
  };
  float data[3];
} vertex_t;

typedef struct {
  int verts[3];
} triangle_t;

//typedef float[3] vertex_t;
//typedef int[3] triangle_t;

//only support TRIS
typedef struct {
  int n_verts;
  int n_tris;
  vertex_t * verts;
  triangle_t * tris;
  //Rendering
  GLuint vbo, ebo;
} model_t;

model_t * obj_read(const char * path);
void obj_delete(model_t * obj);

#endif

