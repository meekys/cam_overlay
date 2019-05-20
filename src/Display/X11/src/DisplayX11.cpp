#include <assert.h>

#include <DisplayX11/DisplayX11.hpp>

DisplayX11::DisplayX11(std::shared_ptr<ILogger> logger, std::shared_ptr<IInput> input)
: _logger(logger), _input(input), _xDisplay(NULL), _screen(0), _rootWindow(0), _window(0), _wmDeleteWindow(0), _width(0), _height(0)
{
    _logger->Info("Initialising DisplayX11...");

    _xDisplay = ::XOpenDisplay(NULL);
    assert(_xDisplay != NULL);
    
    _screen = DefaultScreen(_xDisplay);
    _rootWindow = RootWindow(_xDisplay, _screen);

    _width = input->getWidth();
    _height = input->getHeight();

    _window = ::XCreateSimpleWindow(
        _xDisplay,
        _rootWindow,
        0,
        0,
        _width,
        _height,
        0,
        BlackPixel(_xDisplay, _screen),
        BlackPixel(_xDisplay, _screen));

    assert(_window);

    ::XStoreName(_xDisplay, _window, "cam_overlay");

    ::XSelectInput(_xDisplay, _window, KeyPressMask);

    ::XCreateGC(_xDisplay, _window, 0, NULL);

    ::XMapWindow(_xDisplay, _window);

    _wmDeleteWindow = XInternAtom(_xDisplay, "WM_DELETE_WINDOW", False);
    ::XSetWMProtocols(_xDisplay, _window, &_wmDeleteWindow, 1);

    ::XUndefineCursor(_xDisplay, _window);
    ::XMapRaised(_xDisplay, _window);
    ::XFlush(_xDisplay);
}

DisplayX11::~DisplayX11()
{
    _logger->Info("Cleanup DisplayX11...");

    ::XDestroyWindow(_xDisplay, _window);
    ::XCloseDisplay(_xDisplay);
}

bool DisplayX11::isClosed()
{
    while (XPending(_xDisplay))
    {
        XEvent event;
        ::XNextEvent(_xDisplay, &event);

        switch (event.type)
        {
            case ClientMessage:
                if ((Atom)event.xclient.data.l[0] == _wmDeleteWindow)
                    return true;
                break;

            case KeyPress:
                KeySym keySymbol;
                char buffer;
                ::XLookupString(&event.xkey, &buffer, 1, &keySymbol, NULL);

                if (keySymbol == XK_Escape)
                    return true;

                break;
        }
    }

    return false;
}

NativeWindowType DisplayX11::getWindow()
{
    return _window;
}

uint DisplayX11::getWidth()
{
    return _width;
}

uint DisplayX11::getHeight()
{
    return _height;
}