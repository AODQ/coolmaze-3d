#include "platform.h"
#include "glfns.h"

int main(int argc, char ** argv)
{
  float verts[6] = {0.f, 1.f,  -1.f, -1.f,  1.f, -1.f};
  unsigned int tris[3] = {0, 1, 2};
  GLuint vbo, ebo;
  pl_create_window(640, 480, 0);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glGenBuffers(1, &(vbo));
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, verts, GL_STATIC_DRAW);
  glGenBuffers(1, &(ebo));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3, tris, GL_STATIC_DRAW);
  glClear(GL_COLOR_BUFFER_BIT);
  glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
  glDisableClientState(GL_VERTEX_ARRAY);
  pl_swap_window();
  while(1);
  return 0;
}
