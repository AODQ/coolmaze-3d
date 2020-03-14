#include "glfns.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <wingdi.h>
#else
#include <GL/glx.h>
#include <GL/glxext.h>

#endif


#define X(fn, type) type fn;
  GLFNS_XMACRO
#undef X

int load_opengl_functions(void)
{
  const unsigned char * str = glGetString(GL_VERSION);
  if(strcmp("2.1", str) > 0){
    printf("Unsuitable OpenGL Version: %s\n", glGetString(GL_VERSION));
    return -1;
  }
  #ifdef _WIN32
    #define X(fn, type) fn = (type) wglGetProcAddress(#fn);
  #else
    #define X(fn, type) fn = (type) glXGetProcAddressARB((GLubyte *) #fn);
  #endif

    GLFNS_XMACRO
  #undef X
  return 0;
}
