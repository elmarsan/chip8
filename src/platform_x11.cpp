#include "chip8.h"
#include "platform.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>

Display* display;
Window window;
XEvent event;
GC gc;
int width;
int height;

uint32_t* videoBuff[chip8Width * chip8Height];

bool CreateWindow(const std::string& title, const int w, const int h)
{
    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        std::cout << "Cannot open display\n";
        return false;
    }

    const auto screen = DefaultScreen(display);

    width = w; 
    height = h; 
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, w, h, 0, 0, 0);
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);

    gc = XCreateGC(display, window, 0, NULL);
    return true;
}

bool UpdateWindow(const uint32_t (&videoBuffer)[chip8Width * chip8Height])
{
    XNextEvent(display, &event);

    // Calculate the scale factors
    const auto scaleX = width / chip8Width;
    const auto scaleY = height / chip8Height;

    // Handle window exposure event
    if (event.type == Expose)
    {
        // Draw the buffer onto the window
        for (int y = 0; y < chip8Height; ++y)
        {
            for (int x = 0; x < chip8Width; ++x)
            {
                XSetForeground(display, gc, videoBuffer[y * chip8Width + x]);

                // Calculate the position and size of the rectangle
                const auto rectX = x * scaleX;
                const auto rectY = y * scaleY;
                const auto rectWidth = scaleX;
                const auto rectHeight = scaleY;

                // Draw a filled rectangle to scale the pixel
                XFillRectangle(display, window, gc, rectX, rectY, rectWidth, rectHeight);
            }
        }
        /* XFreeGC(display, gc); */
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
