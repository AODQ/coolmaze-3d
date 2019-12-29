#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "glfns.h"

#include "gfx.h"

//Globals
GLint u_mvp, u_tex;
GLuint prog;

GLuint test_tex;

//tex
GLuint txt_vbo, txt_ebo, txt_tex;
float txt_w;

//MVP
mat_t mvp_val;

struct _model {
  GLuint vbo, ebo, nverts, nelems, tex;
};

struct _billboard {
  GLuint tex;
  float scale;
};

GLuint _ld_shader(GLenum type, const char *filename) {
  FILE* fp;
  GLint sz;
  GLchar* src;
  GLuint shader = glCreateShader(type);
  fp = fopen(filename, "rb");
  fseek(fp, 0, SEEK_END);
  sz = (GLint)ftell(fp);
  rewind(fp);
  src = malloc(sz+1);
  fread(src, sz, 1, fp);
  //Compile
  glShaderSource(shader, 1, (const GLchar * const *) &src, &sz);
  glCompileShader(shader);
  //Test for errors
  GLint ls=0;
  GLchar *log=0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &ls);
  if(ls > 0)
  {
    log = malloc(ls);
    glGetShaderInfoLog(shader, ls, &ls, log);
    printf("%s\n", log);
  }
  if(log) free(log);
  free(src);
  fclose(fp);
  return shader;
}

void g_init(void)
{
  { // create program
    GLuint vert, frag;
    prog = glCreateProgram();
    frag = _ld_shader(GL_FRAGMENT_SHADER, "shaders/frag.glsl");
    vert = _ld_shader(GL_VERTEX_SHADER, "shaders/vert.glsl");
    glAttachShader(prog, frag);
    glAttachShader(prog, vert);
    glLinkProgram(prog);
    glDeleteShader(vert);
    glDeleteShader(frag);
  }

  u_mvp = glGetUniformLocation(prog, "MVP");
  u_tex = glGetUniformLocation(prog, "tex");
  glUseProgram(prog);
  //Test texture
  unsigned char * tex = malloc(4*32*32);
  for(int i = 0; i < 32*32; i++){
    int pink = ((i & 0x04) && (i & 0x80)) || (!(i&0x04) && !(i&0x80));
    tex[i*4] = pink ? 255 : 0;
    tex[i*4 + 1] = 0;
    tex[i*4 + 2] = pink ? 255 : 0;
    tex[i*4 + 3] = 255;
  }
  //Default Texture
  glGenTextures(1, &test_tex);
  glBindTexture(GL_TEXTURE_2D, test_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    32, 32, 0,
    GL_RGBA, GL_UNSIGNED_BYTE,
    tex
  );
  free(tex);
  //glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, test_tex);
  glUniform1i(u_tex, 0);

  //from main
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClearDepth(1.f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  glEnable(GL_POINT_SPRITE);
}

void g_clear(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

g_texture_t g_ld_texture(int w, int h, unsigned char * rgba)
{
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    w, h, 0,
    GL_RGBA, GL_UNSIGNED_BYTE,
    rgba
  );
  //glBindTexture(GL_TEXTURE_2D, 0);
  return tex;
}

//Interleave
g_model_t * g_ld_model(
  unsigned int nverts, float * verts, float * tcs,
  unsigned int nelems, unsigned int * elems,
  g_texture_t tex
){
  g_model_t * m = malloc(sizeof(g_model_t));
  //n
  m->nverts = nverts;
  m->nelems = nelems;
  m->tex = tex ? tex : test_tex;

  //Vertices
  glGenBuffers(1, &(m->vbo));
  glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
  glBufferData(
    GL_ARRAY_BUFFER,
    5 *sizeof(float) * nverts,
    NULL,
    GL_STATIC_DRAW
  );
  for(int i = 0; i < nverts; i++){
    glBufferSubData(
      GL_ARRAY_BUFFER,
      sizeof(float) * (5*i),
      3*sizeof(float),
      verts + i*3
    );
    glBufferSubData(
      GL_ARRAY_BUFFER,
      sizeof(float) * (5*i + 3),
      2*sizeof(float),
      tcs ? tcs + i*2 : verts + i*3
    );
  }

  //Indices
  glGenBuffers(1, &(m->ebo));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    sizeof(unsigned int) * nelems,
    elems,
    GL_STATIC_DRAW
  );
  return m;
}

void g_render_model(g_model_t * m)
{
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, m->tex);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glVertexPointer(3, GL_FLOAT, sizeof(float)*5, (GLvoid *) 0);
    //glNormalPointer(3, GL_FLOAT, 0, 3 * sizeof(float) * m->n_verts);
    glTexCoordPointer(2, GL_FLOAT, sizeof(float)*5, (GLvoid *) (3 * sizeof(float)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
    glDrawElements(GL_TRIANGLES, m->nelems, GL_UNSIGNED_INT, 0);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void g_mvp_mat(mat_t mat)
{
  mvp_val = mat;
  glUniformMatrix4fv(u_mvp, 1, GL_TRUE, (float *) mat.data);
}

void g_set_font(g_texture_t tex, int cw, int ch, int row, int col)
{
  txt_tex = tex;
  //Gen buffers
  float * arr = malloc(sizeof(float) * (4*128*2 + 4*2));
  float width, height;
  width = (float) cw / (2.f*(cw > ch ? cw : ch));
  height = (float) ch / (2.f*(cw > ch ? cw : ch));
  //Vertices
  arr[0] = -width; arr[1] =  height;
  arr[2] = -width; arr[3] = -height;
  arr[4] =  width; arr[5] = -height;
  arr[6] =  width; arr[7] =  height;
  txt_w = 2*width;
  //TexCoords
  for(int i = 0; i < row; i++){
    for(int j = 0; j < col; j++){
      arr[8 + 8*(j + i*col)] =  j / (float) col;
      arr[8 + 8*(j + i*col)+1] =1.0 - (i) / (float) row; 

      arr[8 + 8*(j + i*col)+2] =j / (float) col; 
      arr[8 + 8*(j + i*col)+3] =1.0 - (i+1) / (float) row; 

      arr[8 + 8*(j + i*col)+4] =(j+1) / (float) col; 
      arr[8 + 8*(j + i*col)+5] =1.0 - (i+1) / (float) row; 

      arr[8 + 8*(j + i*col)+6] =(j+1) / (float) col; 
      arr[8 + 8*(j + i*col)+7] =1.0 - (i) / (float) row; 
    }
  }
  glGenBuffers(1, &txt_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, txt_vbo);
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(float) * (row*col + 1) * 8,
    arr, 
    GL_STATIC_DRAW
  );

  //Indices
  //TODO change winding?
  GLuint indices[4] = {0, 1, 2, 3};
  glGenBuffers(1, &txt_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, txt_ebo);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    4*sizeof(GLuint),
    indices,
    GL_STATIC_DRAW
  );
  //cleanup
  free(arr);
}

void g_puts(float x, float y, float scale, const char * str)
{
  mat_t model = {.data = {
    {scale, 0, 0, x},
    {0, scale, 0, y},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
  }};
  glDisable(GL_DEPTH_TEST);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, txt_tex);
    glBindBuffer(GL_ARRAY_BUFFER, txt_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, txt_ebo);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    for(int i = 0; i < strlen(str); i++){
      glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*) (sizeof(float)*8*(1 + (long) str[i])));
      glUniformMatrix4fv(u_mvp, 1, GL_TRUE, (float*) model.data);
      glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
      model.data[0][3] += scale*txt_w;
    }
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glUniformMatrix4fv(u_mvp, 1, GL_TRUE, (float*) mvp_val.data);
  glEnable(GL_DEPTH_TEST);
}

g_billboard_t * g_ld_billboard(float scale, g_texture_t tex)
{
  g_billboard_t * b = malloc(sizeof(g_billboard_t));
  b->scale = scale;
  b->tex = tex ? tex : test_tex;
  return b;
}

void g_render_billboard(g_billboard_t * b)
{
  glPointSize(b->scale);
  glBindTexture(GL_TEXTURE_2D, b->tex);

  glBegin(GL_QUADS);
    glVertex2f(-0.25, 0.25);
    glTexCoord2f(0.0f, 0.f);

    glVertex2f(-0.25, -0.25);
    glTexCoord2f(0.0f, 1.f);

    glVertex2f(0.25, -0.25);
    glTexCoord2f(1.0f, 1.f);

    glVertex2f(0.25, 0.25);
    glTexCoord2f(1.0f, 0.f);
  glEnd();

  /* 
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, bb_vbo);
    glVertexPointer(2, GL_FLLOAT, 0
    glBindBuffer(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bb_ebo);
    glDrawElements(GL_Quads, 4, GL_UNSIGNED_INT, 0);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  */
}

