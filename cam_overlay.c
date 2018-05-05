/*
 *  V4L2 video capture example
 *
 *  This program can be used and distributed without restrictions.
 *
 *      This program is provided with the V4L2 API
 * see http://linuxtv.org/docs.php for more information
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

#include "bcm_host.h"

#include "GLES/gl.h"
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "common.h"
#include "display.h"
#include "png_texture.h"
#include "matrix.h"

#ifndef ALIGN_UP
#define ALIGN_UP(x,y)  ((x + (y)-1) & ~((y)-1))
#endif

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define TEXTURE_BUFFER // Use intermediate texture for decoding to

typedef struct
{
    Matrix4                     matrix;
    GLuint                      program;
    GLuint                      uniform_projection_matrix;
    GLuint                      attrib_vertex;
    GLuint                      attrib_uv;
    GLuint                      uniform_texture;
    GLuint                      uniform_width;

} DECODER_T;

typedef struct
{
    Matrix4                     matrix;
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
    uint32_t                    screen_width;
    uint32_t                    screen_height;
    EGLDisplay                  display;
    EGLContext                  context;
    EGLSurface                  surface;
    DISPMANX_DISPLAY_HANDLE_T   dispman_display;
    DISPMANX_ELEMENT_HANDLE_T   dispman_element;

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

static STATE_T          gState;

enum io_method {
    IO_METHOD_READ,
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR,
};

struct buffer {
    void   *start;
    size_t  length;
};

static char            *dev_name;
static enum io_method   io = IO_METHOD_MMAP;
static int              fd = -1;
struct buffer          *buffers;
static unsigned int     n_buffers;
static int              force_format;

static volatile int running = 1;

static void interruptHandler(int unused) {
    running = 0;
}

static void init_ogl(STATE_T *state, int display, int layer)
{
    int32_t success = 0;
    EGLBoolean result;
    EGLint num_config;

    static EGL_DISPMANX_WINDOW_T nativewindow;

    DISPMANX_UPDATE_HANDLE_T dispman_update;
    VC_RECT_T dst_rect;
    VC_RECT_T src_rect;

    static const EGLint attribute_list[] =
    {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };

    EGLConfig config;

    // get an EGL display connection
    state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(state->display != EGL_NO_DISPLAY);
    check();

    // initialize the EGL display connection
    result = eglInitialize(state->display, NULL, NULL);
    assert(EGL_FALSE != result);
    check();

    // get an appropriate EGL frame buffer configuration
    result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
    assert(EGL_FALSE != result);
    check();

    // // get an appropriate EGL frame buffer configuration
    // result = eglBindAPI(EGL_OPENGL_ES_API);
    // assert(EGL_FALSE != result);
    // check();

    static const EGLint context_attributes[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    // create an EGL rendering context
    state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
    assert(state->context != EGL_NO_CONTEXT);
    check();

    // create an EGL window surface
    success = graphics_get_display_size(display /* LCD */, &state->screen_width, &state->screen_height);
    assert(success >= 0);

    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width = state->screen_width;
    dst_rect.height = state->screen_height;

    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = state->screen_width << 16;
    src_rect.height = state->screen_height << 16;

    state->dispman_display = vc_dispmanx_display_open(display /* LCD */);
    dispman_update = vc_dispmanx_update_start(0);

    state->dispman_element = vc_dispmanx_element_add(
        dispman_update,
        state->dispman_display,
        layer /*layer*/,
        &dst_rect,
        0 /*src*/,
        &src_rect,
        DISPMANX_PROTECTION_NONE,
        0 /*alpha*/,
        0 /*clamp*/,
        0 /*transform*/);

    nativewindow.element = state->dispman_element;
    nativewindow.width = state->screen_width;
    nativewindow.height = state->screen_height;
    vc_dispmanx_update_submit_sync(dispman_update);

    state->surface = eglCreateWindowSurface(state->display, config, &nativewindow, NULL);
    assert(state->surface != EGL_NO_SURFACE);
    check();

    // connect the context to the surface
    result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
    assert(EGL_FALSE != result);
    check();

    fprintf(stderr, "Opened screen %d @ %d x %d\n", display, state->screen_width, state->screen_height);

    glClearColor(0.0f, 0.0f, 1.0f, 0.25f);

    // Set background color and clear buffers
    //   glClearColor(0.15f, 0.25f, 0.35f, 1.0f);

    // Enable back face culling.
    //glEnable(GL_CULL_FACE);

    // glMatrixMode(GL_MODELVIEW);
}

static void init_shaders(STATE_T *state, const char* vertex_filename, const char* fragment_filename, const char* decoder_fragment_filename)
{
    char* vertex_source = read_file(vertex_filename);
    char* fragment_source = read_file(fragment_filename);
    char* decoder_fragment_source = read_file(decoder_fragment_filename);

    state->decoder.program = compile_shader_program(vertex_source, decoder_fragment_source);
    state->decoder.uniform_projection_matrix = glGetUniformLocation(state->decoder.program, "projection_matrix");
    state->decoder.attrib_vertex   = glGetAttribLocation(state->decoder.program, "vertex");
    state->decoder.attrib_uv       = glGetAttribLocation(state->decoder.program, "uv");
    state->decoder.uniform_texture = glGetUniformLocation(state->decoder.program, "texture");
    state->decoder.uniform_width   = glGetUniformLocation(state->decoder.program, "width");
    check();

    state->overlay.program = compile_shader_program(vertex_source, fragment_source);
    state->overlay.uniform_projection_matrix = glGetUniformLocation(state->overlay.program, "projection_matrix");
    state->overlay.attrib_vertex   = glGetAttribLocation(state->overlay.program, "vertex");
    state->overlay.attrib_uv       = glGetAttribLocation(state->overlay.program, "uv");
    state->overlay.uniform_texture = glGetUniformLocation(state->overlay.program, "texture");
    check();

    free(vertex_source);
    free(fragment_source);
    free(decoder_fragment_source);

    check();
}

static GLfloat screenQuad[4][5] = { // x, y, z, u, v
    {-1.0f,  1.0f, 0.0f,    0.0f, 0.0f}, // Top left
    {-1.0f, -1.0f, 0.0f,    0.0f, 1.0f}, // Bottom left
    { 1.0f,  1.0f, 0.0f,    1.0f, 0.0f}, // Top right
    { 1.0f, -1.0f, 0.0f,    1.0f, 1.0f}  // Bottom right
};
static GLfloat overlayQuad[4][5] = { // x, y, z, u, v
    {-1.0f,  1.0f,  2.0f,    0.0f, 1.0f}, // Top left
    {-1.0f, -1.0f,  1.0f,    0.0f, 0.0f}, // Bottom left
    { 1.0f,  1.0f,  2.0f,    1.0f, 1.0f}, // Top right
    { 1.0f, -1.0f,  1.0f,    1.0f, 0.0f}  // Bottom right
};
static const GLfloat renderQuad[4][5] = { // x, y, z, u, v
    {-1.0f,  1.0f, 0.0f,    0.0f, 1.0f}, // Top left
    {-1.0f, -1.0f, 0.0f,    0.0f, 0.0f}, // Bottom left
    { 1.0f,  1.0f, 0.0f,    1.0f, 1.0f}, // Top right
    { 1.0f, -1.0f, 0.0f,    1.0f, 0.0f}  // Bottom right
};

static void adjust_coordinates(STATE_T *state)
{
    // Adjusts coordinates to maintain ratio within (-1, -1) to (1.0, 1.0)
    // Assumes width > height, adjusting x coordinates to compensate for perspective
    //float ratio = (float)state->width / state->height;

    // Or stretches to full screen
    float ratio = (float)state->screen_width / state->screen_height;

    for (int i = 0; i < 4; i++) {
        screenQuad[i][0] *= ratio;
        overlayQuad[i][0] *= ratio;
    }
}

static GLuint create_vertex_buffer(GLuint size, const void* data)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    check();

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    check();

    return buffer;
}

static void init_view(STATE_T *state)
{
    // glViewport(0, 0, (GLsizei)state->screen_width, (GLsizei)state->screen_height);

    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    adjust_coordinates(state);

    GLfloat aspect = (float)state->screen_width/state->screen_height;

    ortho(state->decoder.matrix, -aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    perspective(state->overlay.matrix, 90.0f, aspect, -1.0, 1.0f);

    state->buffer_screen = create_vertex_buffer(sizeof(screenQuad), screenQuad);
    state->buffer_overlay = create_vertex_buffer(sizeof(overlayQuad), overlayQuad);
    state->buffer_render = create_vertex_buffer(sizeof(renderQuad), renderQuad);
}

static void init_textures(STATE_T *state)
{
    // Video texture
    glGenTextures(1, &state->image_texture);
    glBindTexture(GL_TEXTURE_2D, state->image_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, state->width / 2, state->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    check();

    state->overlay_texture = png_texture_load("overlay.png", 0, 0);

    check();

    // Render texture
    glGenTextures(1, &state->render_texture);
    glBindTexture(GL_TEXTURE_2D, state->render_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, state->width, state->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    check();

    // Render buffer
    glGenFramebuffers(1, &state->render_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, state->render_buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, state->render_texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    check();
}

static void draw_overlay_image(STATE_T *state, const GLint vertex_buffer, const Matrix4 matrix, const GLint texture)
{
    glUseProgram(state->overlay.program);
    check();

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    int stride = sizeof(float) * 5;
    glEnableVertexAttribArray(state->overlay.attrib_vertex);
    glVertexAttribPointer(state->overlay.attrib_vertex, 3, GL_FLOAT, 0, stride, BUFFER_OFFSET(0));
    check();

    glEnableVertexAttribArray(state->overlay.attrib_uv);
    glVertexAttribPointer(state->overlay.attrib_uv, 2, GL_FLOAT, 0, stride, BUFFER_OFFSET(sizeof(float) * 3));
    check();

    glUniformMatrix4fv(state->overlay.uniform_projection_matrix, 1, 0, (GLfloat*)matrix);
    check();

    glBindTexture(GL_TEXTURE_2D, texture);
    check();

    glUniform1i(state->overlay.uniform_texture, 0); // TEXTURE0
    check();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
}

static void draw_decoder_image(STATE_T *state, const GLint vertex_buffer, const Matrix4 matrix, const GLint texture)
{
    glUseProgram(state->decoder.program);
    check();

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    int stride = sizeof(float) * 5;
    glEnableVertexAttribArray(state->decoder.attrib_vertex);
    glVertexAttribPointer(state->decoder.attrib_vertex, 3, GL_FLOAT, 0, stride, BUFFER_OFFSET(0));
    check();

    glEnableVertexAttribArray(state->decoder.attrib_uv);
    glVertexAttribPointer(state->decoder.attrib_uv, 2, GL_FLOAT, 0, stride, BUFFER_OFFSET(sizeof(float) * 3));
    check();

    glUniformMatrix4fv(state->decoder.uniform_projection_matrix, 1, 0, (GLfloat*)matrix);
    check();

    glBindTexture(GL_TEXTURE_2D, texture);
    check();

    glUniform1i(state->decoder.uniform_texture, 0); // TEXTURE0
    check();

    glUniform1i(state->decoder.uniform_width, state->width);
    check();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
}

static void draw_screen(STATE_T *state)
{
#ifdef TEXTURE_BUFFER
    glBindFramebuffer(GL_FRAMEBUFFER, state->render_buffer);
    check();
    glViewport(0, 0, state->width, state->height);
    glClear(GL_COLOR_BUFFER_BIT);

    draw_decoder_image(state, state->buffer_render, identitiyMatrix, state->image_texture);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check();
#endif
    glViewport(0, 0, state->screen_width, state->screen_height);
    glClear(GL_COLOR_BUFFER_BIT);

#ifdef TEXTURE_BUFFER
    draw_overlay_image(state, state->buffer_screen, state->decoder.matrix, state->render_texture);
#else
    draw_decoder_image(state, state->buffer_screen, state->decoder.matrix, state->image_texture);
#endif
    draw_overlay_image(state, state->buffer_overlay, state->overlay.matrix, state->overlay_texture);
}

static void update_screen(STATE_T *state)
{
    //draw_buffer(state);
    draw_screen(state);

    eglSwapBuffers(state->display, state->surface);
}

static void update_image(STATE_T *state, const void* imageData)
{
    glBindTexture(GL_TEXTURE_2D, state->image_texture);

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        state->width / 2,
        state->height,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        imageData);

    glBindTexture(GL_TEXTURE_2D, 0);
}

static void close_display(STATE_T *state)
{
    DISPMANX_UPDATE_HANDLE_T dispman_update;
    int s;
    // clear screen
    glClear( GL_COLOR_BUFFER_BIT );
    eglSwapBuffers(state->display, state->surface);

    glDeleteTextures(1, &state->overlay_texture);
    glDeleteTextures(1, &state->image_texture);
    eglDestroySurface(state->display, state->surface);


    dispman_update = vc_dispmanx_update_start(0);
    s = vc_dispmanx_element_remove(dispman_update, state->dispman_element);
    assert(s == 0);
    vc_dispmanx_update_submit_sync(dispman_update);
    s = vc_dispmanx_display_close(state->dispman_display);
    assert (s == 0);

    // Release OpenGL resources
    eglMakeCurrent(state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(state->display, state->context);
    eglTerminate(state->display);
}

static int xioctl(int fh, unsigned long int request, void *arg)
{
    int r;

    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

static void process_image(const void *p, int size)
{
    STATE_T *state = &gState;

    update_image(state, p);
    update_screen(state);
}

static int read_frame(void)
{
    struct v4l2_buffer buf;
    unsigned int i;

    switch (io) {
    case IO_METHOD_READ:
        if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
            switch (errno) {
            case EAGAIN:
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                errno_exit("read");
            }
        }

        process_image(buffers[0].start, buffers[0].length);
        break;

    case IO_METHOD_MMAP:
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
            switch (errno) {
            case EAGAIN:
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                errno_exit("VIDIOC_DQBUF");
            }
        }

        assert(buf.index < n_buffers);

        process_image(buffers[buf.index].start, buf.bytesused);

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
            errno_exit("VIDIOC_QBUF");
        break;

    case IO_METHOD_USERPTR:
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;

        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
            switch (errno) {
            case EAGAIN:
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                errno_exit("VIDIOC_DQBUF");
            }
        }

        for (i = 0; i < n_buffers; ++i)
            if (buf.m.userptr == (unsigned long)buffers[i].start
                && buf.length == buffers[i].length)
                break;

        assert(i < n_buffers);

        process_image((void *)buf.m.userptr, buf.bytesused);

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
            errno_exit("VIDIOC_QBUF");
        break;
    }

    return 1;
}

static void mainloop(void)
{
    signal(SIGINT, interruptHandler);

    while (running) {
        for (;;) {
            fd_set fds;
            struct timeval tv;
            int r;

            FD_ZERO(&fds);
            FD_SET(fd, &fds);

            /* Timeout. */
            tv.tv_sec = 2;
            tv.tv_usec = 0;

            r = select(fd + 1, &fds, NULL, NULL, &tv);

            if (-1 == r) {
                if (EINTR == errno)
                    continue;
                errno_exit("select");
            }

            if (0 == r) {
                fprintf(stderr, "select timeout\n");
                exit(EXIT_FAILURE);
            }

            if (read_frame())
                break;
            /* EAGAIN - continue select loop. */
        }
    }

    fprintf(stderr, "Quitting...\n");
}

static void stop_capturing(void)
{
    enum v4l2_buf_type type;

    switch (io) {
    case IO_METHOD_READ:
        /* Nothing to do. */
        break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
            errno_exit("VIDIOC_STREAMOFF");
        break;
    }
}

static void start_capturing(void)
{
    unsigned int i;
    enum v4l2_buf_type type;

    switch (io) {
    case IO_METHOD_READ:
        /* Nothing to do. */
        break;

    case IO_METHOD_MMAP:
        for (i = 0; i < n_buffers; ++i) {
            struct v4l2_buffer buf;

            CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;

            if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                errno_exit("VIDIOC_QBUF");
        }
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
            errno_exit("VIDIOC_STREAMON");
        break;

    case IO_METHOD_USERPTR:
        for (i = 0; i < n_buffers; ++i) {
            struct v4l2_buffer buf;

            CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_USERPTR;
            buf.index = i;
            buf.m.userptr = (unsigned long)buffers[i].start;
            buf.length = buffers[i].length;

            if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                errno_exit("VIDIOC_QBUF");
        }
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
            errno_exit("VIDIOC_STREAMON");
        break;
    }
}

static void uninit_device(void)
{
    unsigned int i;

    switch (io) {
    case IO_METHOD_READ:
        free(buffers[0].start);
        break;

    case IO_METHOD_MMAP:
        for (i = 0; i < n_buffers; ++i)
            if (-1 == munmap(buffers[i].start, buffers[i].length))
                errno_exit("munmap");
        break;

    case IO_METHOD_USERPTR:
        for (i = 0; i < n_buffers; ++i)
            free(buffers[i].start);
        break;
    }

    free(buffers);
}

static void init_read(unsigned int buffer_size)
{
    buffers = calloc(1, sizeof(*buffers));

    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    buffers[0].length = buffer_size;
    buffers[0].start = malloc(buffer_size);

    if (!buffers[0].start) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
}

static void init_mmap(void)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support "
                 "memory mapping\n", dev_name);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_REQBUFS");
        }
    }

    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on %s\n",
             dev_name);
        exit(EXIT_FAILURE);
    }

    buffers = calloc(req.count, sizeof(*buffers));

    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = n_buffers;

        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
            errno_exit("VIDIOC_QUERYBUF");

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start =
            mmap(NULL /* start anywhere */,
                  buf.length,
                  PROT_READ | PROT_WRITE /* required */,
                  MAP_SHARED /* recommended */,
                  fd, buf.m.offset);

        if (MAP_FAILED == buffers[n_buffers].start)
            errno_exit("mmap");
    }
}

static void init_userp(unsigned int buffer_size)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count  = 4;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support "
                 "user pointer i/o\n", dev_name);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_REQBUFS");
        }
    }

    buffers = calloc(4, sizeof(*buffers));

    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
        buffers[n_buffers].length = buffer_size;
        buffers[n_buffers].start = malloc(buffer_size);

        if (!buffers[n_buffers].start) {
            fprintf(stderr, "Out of memory\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void init_device(STATE_T* state)
{
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;

    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n",
                 dev_name);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_QUERYCAP");
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "%s is no video capture device\n",
             dev_name);
        exit(EXIT_FAILURE);
    }

    switch (io) {
    case IO_METHOD_READ:
        if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
            fprintf(stderr, "%s does not support read i/o\n",
                 dev_name);
            exit(EXIT_FAILURE);
        }
        break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
            fprintf(stderr, "%s does not support streaming i/o\n",
                 dev_name);
            exit(EXIT_FAILURE);
        }
        break;
    }


    /* Select video input, video standard and tune here. */


    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
            switch (errno) {
            case EINVAL:
                /* Cropping not supported. */
                break;
            default:
                /* Errors ignored. */
                break;
            }
        }
    } else {
        /* Errors ignored. */
    }

    CLEAR(fmt);

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (force_format) {
        fmt.fmt.pix.width       = 640;
        fmt.fmt.pix.height      = 480;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
            errno_exit("VIDIOC_S_FMT");

        /* Note VIDIOC_S_FMT may change width and height. */
    }

    if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
        errno_exit("VIDIOC_G_FMT");

    int format[2];
    CLEAR(format);
    format[0] = fmt.fmt.pix.pixelformat;

    fprintf(stderr, "Dimensions: %d x %d Pixel Format: %s\n",
             fmt.fmt.pix.width, fmt.fmt.pix.height, (char*)format);

    state->width = fmt.fmt.pix.width;
    state->height = fmt.fmt.pix.height;

    switch (io) {
    case IO_METHOD_READ:
        init_read(fmt.fmt.pix.sizeimage);
        break;

    case IO_METHOD_MMAP:
        init_mmap();
        break;

    case IO_METHOD_USERPTR:
        init_userp(fmt.fmt.pix.sizeimage);
        break;
    }
}

static void close_device(void)
{
    if (-1 == close(fd))
        errno_exit("close");

    fd = -1;
}

static void open_device(void)
{
    struct stat st;

    if (-1 == stat(dev_name, &st)) {
        fprintf(stderr, "Cannot identify '%s': %d, %s\n",
             dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (!S_ISCHR(st.st_mode)) {
        fprintf(stderr, "%s is no device\n", dev_name);
        exit(EXIT_FAILURE);
    }

    fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

    if (-1 == fd) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n",
             dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Opened device '%s'\n", dev_name);
}

static void usage(FILE *fp, int argc, char **argv)
{
    fprintf(fp,
         "Usage: %s [options]\n\n"
         "Version 1.3\n"
         "Options:\n"
         "-d | --device name   Video device name [%s]\n"
         "-h | --help          Print this message\n"
         "-m | --mmap          Use memory mapped buffers [default]\n"
         "-r | --read          Use read() calls\n"
         "-u | --userp         Use application allocated buffers\n"
         "",
         argv[0], dev_name);
}

static const char short_options[] = "d:hmruofc:";

static const struct option
long_options[] = {
    { "device", required_argument, NULL, 'd' },
    { "help",   no_argument,       NULL, 'h' },
    { "mmap",   no_argument,       NULL, 'm' },
    { "read",   no_argument,       NULL, 'r' },
    { "userp",  no_argument,       NULL, 'u' },
    { 0, 0, 0, 0 }
};

int main(int argc, char **argv)
{
    dev_name = "/dev/video0";

    for (;;) {
        int idx;
        int c;

        c = getopt_long(argc, argv,
                short_options, long_options, &idx);

        if (-1 == c)
            break;

        switch (c) {
        case 0: /* getopt_long() flag */
            break;

        case 'd':
            dev_name = optarg;
            break;

        case 'h':
            usage(stdout, argc, argv);
            exit(EXIT_SUCCESS);

        case 'm':
            io = IO_METHOD_MMAP;
            break;

        case 'r':
            io = IO_METHOD_READ;
            break;

        case 'u':
            io = IO_METHOD_USERPTR;
            break;

        default:
            usage(stderr, argc, argv);
            exit(EXIT_FAILURE);
        }
    }

    CLEAR(gState);

    open_device();
    init_device(&gState);

    bcm_host_init();
    init_ogl(&gState, 0, 10);
    init_shaders(&gState, "shader.vert", "shader.frag", "shader-YUYV.frag");
    init_view(&gState);
    init_textures(&gState);

    start_capturing();

    mainloop();

    stop_capturing();

    close_display(&gState);

    uninit_device();
    close_device();

    fprintf(stderr, "\n");

    return 0;
}
