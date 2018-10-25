#ifndef COMMON_H
#define COMMON_H

#include "gl_common.h"
#include "math_3d.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

extern void errno_exit(const char *s);
extern void gl_exit(const char *s);
extern char* read_file(const char* filename);

#if DEBUG
extern int log_verbose_enabled;

#define log_verbose(fmt, ...) \
if (log_verbose_enabled) \
    do { fprintf(stderr, "%s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while (0)
#else
    #define log_verbose(fmt, ...) {}
#endif

typedef struct
{
    mat4_t                      matrix;
    GLuint                      program;
    GLuint                      uniform_projection_matrix;
    GLuint                      attrib_vertex;
    GLuint                      attrib_uv;
    GLuint                      uniform_texture;
    GLuint                      uniform_width;

} DECODER_T;

typedef struct
{
    mat4_t                      matrix;
    GLuint                      program;
    GLuint                      uniform_projection_matrix;
    GLuint                      attrib_vertex;
    GLuint                      attrib_uv;
    GLuint                      uniform_texture;

} OVERLAY_T;

typedef struct
{
    int                         width;
    int                         height;
    char*                       format;
    
    uint32_t                    screen_width;
    uint32_t                    screen_height;
    EGLDisplay                  display;
    EGLContext                  context;
    EGLSurface                  surface;

    DECODER_T                   decoder;
    OVERLAY_T                   overlay;

    GLuint                      image_texture;
    GLuint                      render_texture;
    GLuint                      overlay_texture;

    GLuint                      render_buffer;

    // vertex + uv buffer
    GLuint                      buffer_screen;
    GLuint                      buffer_overlay;
    GLuint                      buffer_render;
} STATE_T;

#endif