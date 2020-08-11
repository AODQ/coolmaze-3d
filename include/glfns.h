#ifndef GLFNS_H
#define GLFNS_H

#ifdef _WIN32
#include "windows.h"
#else
#include <GL/glx.h>
#endif
#include <GL/gl.h>
#include <GL/glext.h>

#define GLFNS_XMACRO \
X(glGenBuffers, PFNGLGENBUFFERSPROC) \
X(glBindBuffer, PFNGLBINDBUFFERPROC) \
X(glBufferData, PFNGLBUFFERDATAPROC) \
X(glBufferSubData, PFNGLBUFFERSUBDATAPROC) \
X(glCreateShader, PFNGLCREATESHADERPROC) \
X(glDeleteShader, PFNGLDELETESHADERPROC) \
X(glShaderSource, PFNGLSHADERSOURCEPROC) \
X(glCompileShader, PFNGLCOMPILESHADERPROC) \
X(glGetShaderiv, PFNGLGETSHADERIVPROC) \
X(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC) \
X(glCreateProgram, PFNGLCREATEPROGRAMPROC) \
X(glAttachShader, PFNGLATTACHSHADERPROC) \
X(glLinkProgram, PFNGLLINKPROGRAMPROC) \
X(glUseProgram, PFNGLUSEPROGRAMPROC) \
X(glUniform1f, PFNGLUNIFORM1FPROC) \
X(glUniform2f, PFNGLUNIFORM2FPROC) \
X(glUniform3f, PFNGLUNIFORM3FPROC) \
X(glUniform4f, PFNGLUNIFORM4FPROC) \
X(glUniform1i, PFNGLUNIFORM1IPROC) \
X(glUniform2i, PFNGLUNIFORM2IPROC) \
X(glUniform3i, PFNGLUNIFORM3IPROC) \
X(glUniform4i, PFNGLUNIFORM4IPROC) \
X(glUniformMatrix2fv, PFNGLUNIFORMMATRIX2FVPROC) \
X(glUniformMatrix3fv, PFNGLUNIFORMMATRIX3FVPROC) \
X(glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC) \
X(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC) \
X(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC) \
X(glDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYPROC)

#define GLFNS_X_POSIX_MACRO \
X(glXSwapIntervalEXT, PFNGLXSWAPINTERVALEXTPROC)

/*
X(glDrawElements, PFNGLDRAWELEMENTSPROC) \
X(glDrawArrays, PFNGLDRAWARRAYSPROC) \
*/

//Prototype GL functions done by glext.h
#define X(fn, type) extern type fn;
GLFNS_XMACRO
#undef X

#ifdef __unix__
#define X(fn, type) extern type fn;
GLFNS_X_POSIX_MACRO
#undef X
#endif

int load_opengl_functions(void);

#endif

