#pragma once

#include <X11/Xlib.h>
#include <memory>

#include <CamOverlay/IDisplay.hpp>
#include <Core/ILogger.hpp>
#include <CamOverlay/IInput.hpp>

#include <Gl/GlCommon.hpp>

class DisplayX11 : public IDisplay
{
public:
    DisplayX11(std::shared_ptr<ILogger> logger, std::shared_ptr<IInput> input);
    virtual ~DisplayX11();

    virtual bool isClosed() override;
    virtual NativeWindowType getWindow() override;
    virtual unsigned int getWidth() override;
    virtual unsigned int getHeight() override;

private:
    std::shared_ptr<ILogger> _logger;
    std::shared_ptr<IInput>  _input;

    Display*     _xDisplay;
    int          _screen;
    Window       _rootWindow;
    Window       _window;
    Atom         _wmDeleteWindow;
    unsigned int _width;
    unsigned int _height;
};