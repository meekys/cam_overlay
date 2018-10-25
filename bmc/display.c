#include <stdio.h>

#include "bcm_host.h"

#include "../common.h"

DISPMANX_DISPLAY_HANDLE_T dispman_display;
DISPMANX_ELEMENT_HANDLE_T dispman_element;

extern void init_display(STATE_T *state, int display, int layer)
{
    bcm_host_init();
}

extern NativeWindowType init_window(STATE_T *state, int display, int layer)
{
    DISPMANX_UPDATE_HANDLE_T dispman_update;
    VC_RECT_T dst_rect;
    VC_RECT_T src_rect;

    // create an EGL window surface
    success = graphics_get_display_size(display /* LCD */, &state->screen_width, &state->screen_height);
    assert(success >= 0);

    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width = state->screen_width;
    dst_rect.height = state->screen_height;

    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = state->screen_width << 16;
    src_rect.height = state->screen_height << 16;

    dispman_display = vc_dispmanx_display_open(display /* LCD */);
    dispman_update = vc_dispmanx_update_start(0);

    dispman_element = vc_dispmanx_element_add(
        dispman_update,
        dispman_display,
        layer /*layer*/,
        &dst_rect,
        0 /*src*/,
        &src_rect,
        DISPMANX_PROTECTION_NONE,
        0 /*alpha*/,
        0 /*clamp*/,
        0 /*transform*/);

    static EGL_DISPMANX_WINDOW_T nativewindow;
    nativewindow.element = dispman_element;
    nativewindow.width = state->screen_width;
    nativewindow.height = state->screen_height;
    vc_dispmanx_update_submit_sync(dispman_update);

    return &nativewindow;
}

extern void close_display(STATE_T *state)
{
    DISPMANX_UPDATE_HANDLE_T dispman_update;
    int s;

    dispman_update = vc_dispmanx_update_start(0);
    s = vc_dispmanx_element_remove(dispman_update, state->dispman_element);
    assert(s == 0);
    vc_dispmanx_update_submit_sync(dispman_update);
    s = vc_dispmanx_display_close(state->dispman_display);
    assert (s == 0);
}