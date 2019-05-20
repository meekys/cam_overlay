#include <assert.h>

#include <Gl/Context.hpp>

using namespace Gl;

Context::Context(NativeWindowType window)
{
    // get an EGL display connection
    _display = ::eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(_display != EGL_NO_DISPLAY);
    check();

    // initialize the EGL display connection
    auto result = ::eglInitialize(_display, NULL, NULL);
    assert(EGL_FALSE != result);
    check();

    static const EGLint attribute_list[] =
    {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        //EGL_DEPTH_SIZE, 16,
        //EGL_STENCIL_SIZE, 0,
        //EGL_SAMPLE_BUFFERS, 1,
        //EGL_SAMPLES, 4,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };

    // get an appropriate EGL frame buffer configuration
    EGLConfig config;
    EGLint num_config;
    result = ::eglChooseConfig(_display, attribute_list, &config, 1, &num_config);
    assert(EGL_FALSE != result);
    assert(num_config == 1);
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
    _context = ::eglCreateContext(_display, config, EGL_NO_CONTEXT, context_attributes);
    assert(_context != EGL_NO_CONTEXT);
    check();

    _surface = ::eglCreateWindowSurface(_display, config, window, NULL);
    assert(_surface != EGL_NO_SURFACE);
    check();

    // connect the context to the surface
    result = ::eglMakeCurrent(_display, _surface, _surface, _context);
    assert(EGL_FALSE != result);
    check();

    ::glClearColor(0.0f, 0.0f, 1.0f, 0.25f);
}

Context::~Context()
{
    ::eglSwapBuffers(_display, _surface);

    ::eglDestroySurface(_display, _surface);

    // Release OpenGL resources
    ::eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    ::eglDestroyContext(_display, _context);
    ::eglTerminate(_display);
}

void Context::SwapBuffers()
{
    ::eglSwapBuffers(_display, _surface);
}