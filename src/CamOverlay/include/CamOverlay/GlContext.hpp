#pragma once

#include <Core/ILogger.hpp>
#include <CamOverlay/IDisplay.hpp>
#include <Gl/Context.hpp>

class GlContext
{
public:
    GlContext(std::shared_ptr<ILogger> logger, std::shared_ptr<IDisplay> display);
    ~GlContext();

    void SwapBuffers();

private:
    std::shared_ptr<ILogger>     _logger;
    std::shared_ptr<IDisplay>    _display;
    std::shared_ptr<Gl::Context> _glContext;
};