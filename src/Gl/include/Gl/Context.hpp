#pragma once

#include <memory>

#include "GlCommon.hpp"

namespace Gl
{

class Context
{
public:
    Context(NativeWindowType window);
    ~Context();
    void SwapBuffers();

private:
    EGLDisplay _display;
    EGLContext _context;
    EGLSurface _surface;
};

} // namespace Gl