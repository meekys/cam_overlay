#pragma once

#include <Gl/GlCommon.hpp>

class IDisplay
{
public:
    virtual bool isClosed() = 0;
    virtual NativeWindowType getWindow() = 0;
    virtual unsigned int getWidth() = 0;
    virtual unsigned int getHeight() = 0;
};