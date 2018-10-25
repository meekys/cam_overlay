#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "gl_common.h"

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
    log_verbose("glCreateShader");
    GLuint shader = glCreateShader(shaderType);
    assert(shader != 0);

    log_verbose("glShaderSource");
    glShaderSource(shader, 1, (const GLchar **)&source, 0);
    log_verbose("glCompileShader");
    glCompileShader(shader);
    check();

    GLint isCompiled;
    log_verbose("glGetShaderiv");
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

    log_verbose("glCreateProgram");
    GLuint program = glCreateProgram();
    assert(program != 0);

    log_verbose("glAttachShader: vertex_shader");
    glAttachShader(program, vertex_shader);
    check();
    log_verbose("glAttachShader: fragment_shader");
    glAttachShader(program, fragment_shader);
    check();
    log_verbose("glLinkProgram");
    glLinkProgram(program);
    check();

    GLint isCompiled;
    log_verbose("glGetProgramiv");
    glGetProgramiv(program, GL_LINK_STATUS, &isCompiled);
    if (isCompiled != GL_TRUE)
    {
        showprogramlog(program);
        exit(EXIT_FAILURE);
    }

    log_verbose("glDeleteShader: vertex_shader");
    glDeleteShader(vertex_shader);
    log_verbose("glDeleteShader: fragment_shader");
    glDeleteShader(fragment_shader);

    return program;
}