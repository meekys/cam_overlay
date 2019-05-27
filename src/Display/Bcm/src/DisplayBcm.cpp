#include <assert.h>

#include <DisplayBcm/DisplayBcm.hpp>

DisplayBcm::DisplayBcm(std::shared_ptr<ILogger> logger)
: _logger(logger), _width(0), _height(0)
{
    _logger->Info("Initialising DisplayBcm...");

    bcm_host_init();

    // create an EGL window surface
    auto display = 0;
    auto layer = 10;
    auto success = graphics_get_display_size(display /* LCD */, &_width, &_height);
    assert(success >= 0);

    VC_RECT_T dst_rect;
    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width = _width;
    dst_rect.height = _height;

    VC_RECT_T src_rect;
    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = _width << 16;
    src_rect.height = _height << 16;

    _display = vc_dispmanx_display_open(display /* LCD */);
    auto update = vc_dispmanx_update_start(0);

    _element = vc_dispmanx_element_add(
        update,
        _display,
        layer /*layer*/,
        &dst_rect,
        0 /*src*/,
        &src_rect,
        DISPMANX_PROTECTION_NONE,
        0 /*alpha*/,
        0 /*clamp*/,
        (DISPMANX_TRANSFORM_T)0 /*transform*/);

    vc_dispmanx_update_submit_sync(update);

    _window.element = _element;
    _window.width = _width;
    _window.height = _height;
}

DisplayBcm::~DisplayBcm()
{
    _logger->Info("Cleanup DisplayBcm...");

    auto update = vc_dispmanx_update_start(0);
    auto s = vc_dispmanx_element_remove(update, _element);
    assert(s == 0);

    vc_dispmanx_update_submit_sync(update);
    s = vc_dispmanx_display_close(_display);
    assert (s == 0);
}

bool DisplayBcm::isClosed()
{
    return false;
}

NativeWindowType DisplayBcm::getWindow()
{
    return &_window;
}

uint DisplayBcm::getWidth()
{
    return _width;
}

uint DisplayBcm::getHeight()
{
    return _height;
}
