#pragma once

#include <assert.h>

#include "GL/gl.h"
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#define check() assert(glGetError() == 0)
#define BUFFER_OFFSET(i) ((char *)NULL + (i))