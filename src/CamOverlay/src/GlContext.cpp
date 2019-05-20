#include <CamOverlay/GlContext.hpp>

GlContext::GlContext(std::shared_ptr<ILogger> logger, std::shared_ptr<IDisplay> display)
    : _logger(logger), _display(display)
{
    _logger->Info("Initialising GL context...");

    _glContext = std::make_shared<Gl::Context>(_display->getWindow());

    ::glEnable(GL_BLEND);
    ::glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    ::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    check();
}

GlContext::~GlContext()
{
    _logger->Info("Cleaning up GL context...");
}

void GlContext::SwapBuffers()
{
    _glContext->SwapBuffers();
}