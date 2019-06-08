#pragma once

#include <assert.h>

#if RASPBERRY_PI
  #include <GLES/gl.h>
  #define GL_RED GL_LUMINANCE
#else
  #include "GL/gl.h"
#endif
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#define check() assert(glGetError() == 0)
#define BUFFER_OFFSET(i) ((char *)NULL + (i))