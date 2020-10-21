#include <assert.h>
#include <Core/Exceptions/Exception.hpp>

#include <DisplayBcm/DisplayBcm.hpp>

DisplayBcm::DisplayBcm(std::shared_ptr<ILogger> logger)
: _logger(logger), _width(0), _height(0)
{
    _logger->Info("Initialising DisplayBcm...");

    bcm_host_init();

    // create an EGL window surface
    auto layer = 10;
    
    DISPMANX_DISPLAY_HANDLE_T display
      = vc_dispmanx_display_open(0 /* LCD */);

    if (display == 0)
    {
        throw Exception("cannot open display");
    }

    DISPMANX_MODEINFO_T info;

    if (vc_dispmanx_display_get_info(display, &info) != 0)
    {
        throw Exception("cannot get display dimensions");
    }
    printf("Size: %dx%d, Transform: %d\n", info.width, info.height, info.transform);

    VC_RECT_T dst_rect;
    VC_RECT_T src_rect;
    VC_IMAGE_TYPE_T type = VC_IMAGE_YUV420;
    DISPMANX_RESOURCE_HANDLE_T resource;
    DISPMANX_ELEMENT_HANDLE_T element;
    uint32_t vc_image_ptr;
    
    vc_dispmanx_rect_set(&dst_rect, 0, 0, info.width, info.height);
    vc_dispmanx_rect_set(&src_rect, 0, 0, info.width << 16, info.height << 16);
    
    resource = vc_dispmanx_resource_create(type, 1, 1, &vc_image_ptr);
    if (resource != 0)
    {
        throw Exception("Could not create resource");
    }

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);

    element = vc_dispmanx_element_add(
        update,
        display,
        layer /*layer*/,
        &dst_rect,
        0 /*src*/,
        &src_rect,
        DISPMANX_PROTECTION_NONE,
        0 /*alpha*/,
        0 /*clamp*/,
        (DISPMANX_TRANSFORM_T)0 /*transform*/);

    vc_dispmanx_update_submit_sync(update);

    _window.element = element;
    _window.width = info.width;
    _window.height = info.height;
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
