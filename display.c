#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "GLES/gl.h"
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "common.h"
#include "display.h"

static void showlog(GLint shader)
{
   // Prints the compile log for a shader
   char log[1024];
   CLEAR(log);
   glGetShaderInfoLog(shader, sizeof log, NULL, log);
   fprintf(stderr, "%d:shader:\n%s\n", shader, log);
}

static void showprogramlog(GLint shader)
{
   // Prints the information log for a program object
   char log[1024];
   glGetProgramInfoLog(shader,sizeof log, NULL, log);
   fprintf(stderr, "%d:program:\n%s\n", shader, log);
}

static GLuint create_shader(const char* source, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    assert(shader != 0);

    glShaderSource(shader, 1, (const GLchar **)&source, 0);
    glCompileShader(shader);
    check();

    GLint isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled  != GL_TRUE)
    {
        showlog(shader);
        exit(EXIT_FAILURE);
    }

    return shader;
}

extern GLint compile_shader_program(char* vertex_source, char* fragment_source)
{
    GLuint vertex_shader = create_shader(vertex_source, GL_VERTEX_SHADER);
    GLuint fragment_shader = create_shader(fragment_source, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    assert(program != 0);

    glAttachShader(program, vertex_shader);
    check();
    glAttachShader(program, fragment_shader);
    check();
    glLinkProgram(program);
    check();

    GLint isCompiled;
    glGetProgramiv(program, GL_LINK_STATUS, &isCompiled);
    if (isCompiled != GL_TRUE)
    {
        showprogramlog(program);
        exit(EXIT_FAILURE);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}