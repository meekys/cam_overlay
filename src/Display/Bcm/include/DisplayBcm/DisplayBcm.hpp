#pragma once

#include <bcm_host.h>
#include <memory>

#include <CamOverlay/IDisplay.hpp>
#include <Core/ILogger.hpp>
#include <CamOverlay/IInput.hpp>

#include <Gl/GlCommon.hpp>

class DisplayBcm : public IDisplay
{
public:
    DisplayBcm(std::shared_ptr<ILogger> logger);
    virtual ~DisplayBcm();

    virtual bool isClosed() override;
    virtual NativeWindowType getWindow() override;
    virtual unsigned int getWidth() override;
    virtual unsigned int getHeight() override;

private:
    std::shared_ptr<ILogger> _logger;

    DISPMANX_DISPLAY_HANDLE_T _display;
    DISPMANX_ELEMENT_HANDLE_T _element;
    EGL_DISPMANX_WINDOW_T     _window;

    unsigned int _width;
    unsigned int _height;
};
