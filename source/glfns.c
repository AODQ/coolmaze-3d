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
  if(strcmp("2.1", (char*) glGetString(GL_VERSION)) > 0){
    printf("Unsuitable OpenGL Version: %s\n", glGetString(GL_VERSION));
    return -1;
  }
  #ifdef _WIN32
  #define X(fn, type) fn = (type) wglGetProcAddress(#fn);
  #else 
  //Don't have to query extensions for core
  #define X(fn, type) fn = (type) glXGetProcAddressARB((GLubyte *) #fn);
    GLFNS_XMACRO
  #undef X
  #endif
  return 0;
}
