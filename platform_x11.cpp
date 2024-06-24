#include "platform.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>

Display* display;
Window window;
GC context;

uint32_t videoBuff[64 * 32];

bool CreateWindow(const std::string& title, const int width, const int height)
{
    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        std::cout << "Cannot open display\n";
        return false;
    }

    int screen = DefaultScreen(display);

    window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, width, height, 0, 0, 0);
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);

    context = XCreateGC(display, window, 0, NULL);
    return true;
}

bool UpdateWindow()
{
    XEvent event;
    XNextEvent(display, &event);

    // Handle window exposure event
    if (event.type == Expose)
    {
        // Draw the buffer onto the window
        for (int y = 0; y < 32; ++y)
        {
            for (int x = 0; x < 64; ++x)
            {
                XSetForeground(display, context, videoBuff[y * 64 + x]);
                XDrawPoint(display, window, context, x, y);
            }
        }
    }

    // Handle key press event (ESC key)
    if (event.type == KeyPress)
    {
        if (XLookupKeysym(&event.xkey, 0) == XK_Escape)
        {
            CloseWindow();
        }
    }
    return true;
}

void CloseWindow()
{
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}
