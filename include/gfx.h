#ifndef GFX_H
#define GFX_H

#include "vmath.h"

typedef struct _model g_model_t;
typedef unsigned int g_texture_t;
typedef struct _billboard g_billboard_t;

void g_init(void);

g_texture_t g_ld_texture(int w, int h, unsigned char * rgba);

void g_clear(void);

void g_set_font(g_texture_t, int w, int h, int row, int col);

void g_puts(float x, float y, float scale, const char * str);

g_model_t * g_ld_model(
  unsigned int verts, float * v, float * tc,
  unsigned int nelems, unsigned int * indices, 
  g_texture_t tex
);

void g_render_model(g_model_t *);

g_billboard_t * g_ld_billboard(
  float scale,
  g_texture_t tex
);

void g_render_billboard(g_billboard_t *);

void g_mvp_mat(mat_t mat);

#endif

