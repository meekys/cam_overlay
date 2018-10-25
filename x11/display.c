#include <stdio.h>
#include <assert.h>
#include <X11/Xlib.h>

#include "../common.h"

Display* display = NULL;
int screen = 0;
Window rootWindow;
Window window;

extern void init_display(STATE_T *state, int display_unused, int layer)
{
}

extern NativeWindowType init_window(STATE_T *state, int display_unused, int layer)
{
    log_verbose("XOpenDisplay");
    display = XOpenDisplay(NULL);
    assert(display != NULL);
    
    log_verbose("DefaultScreen");
    screen = DefaultScreen(display);
    log_verbose("RootWindow");
    rootWindow = RootWindow(display, screen);

    int width = 1280;
    int height = 480;

    state->screen_width = width;
    state->screen_height = height;

    log_verbose("XCreateSimpleWindow");
    window = XCreateSimpleWindow(
        display,
        rootWindow,
        0,
        0,
        width,
        height,
        0,
        BlackPixel(display, screen),
        BlackPixel(display, screen));

    assert(window);

    log_verbose("XStoreName");
    XStoreName(display, window, "cam_overlay");

    XCreateGC(display, window, 0, NULL);
    //XSetForeground(dpy, gc, WhitePixel(dpy, scr));

    //XSelectInput(dpy, win, ExposureMask | ButtonPressMask);

    log_verbose("XMapWindow");
    XMapWindow(display, window);

    XUndefineCursor(display, window);
    XMapRaised(display, window);
    XFlush(display);

    return window;
}

extern void close_display(STATE_T *state)
{
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}