#include "chip8.h"
#include "input.h"
#include "platform.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include <iostream>

Display* display;
Window window;
GC gc;
int width;
int height;

// Input handling helper
int toggleKey(int keysym, bool pressed);

bool platform_create_window(const std::string& title, const int w, const int h)
{
    // Use null to get the default display of the system.
    display = XOpenDisplay(NULL);
    if (display == nullptr)
    {
        std::cout << "Unable to open X11 display\n";
        return false;
    }

    int screenNumber = DefaultScreen(display);
    const auto displayWidth = XDisplayWidth(display, screenNumber);
    const auto displayHeight = XDisplayHeight(display, screenNumber);

    width = w;
    height = h;
    const int x = displayWidth / 2 - width;
    const int y = displayHeight / 2 - height;

    window = XCreateSimpleWindow(display,                 // display
                                 RootWindow(display, 0),  // window
                                 x,                       // x
                                 y,                       // y
                                 width,                   // width
                                 height,                  // height
                                 0,                       // border_width
                                 0,                       // border,
                                 0                        // background
    );
    if (!window)
    {
        std::cout << "Unable to create X11 window\n";
        return false;
    }

    // Handle resize events and keyboard input.
    XSetWindowAttributes windowAttributes;
    windowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask;
    if (!XChangeWindowAttributes(display, window, CWEventMask, &windowAttributes))
    {
        std::cout << "Unable to set X11 window attributes\n";
        return false;
    }

    // Make window visible
    if (!XMapWindow(display, window))
    {
        std::cout << "Unable to map X11 window\n";
        return false;
    }

    gc = XCreateGC(display, window, 0, nullptr);
    return true;
}

bool platform_update_window(const uint32_t (&videoBuffer)[chip8Width * chip8Height])
{
    if (XPending(display))
    {
        XEvent event;
        XNextEvent(display, &event);

        switch (event.type)
        {
        case KeyPress:
        {
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            toggleKey(keysym, true);
            break;
        }
        case KeyRelease:
        {
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            toggleKey(keysym, false);
            break;
        }
        case Expose:
        {
            width = event.xexpose.width;
            height = event.xexpose.height;
            break;
        }
        case DestroyNotify:
        {
            std::cout << "Closing X11 window...\n";
            return false;
        }
        }

        return true;
    }

    const int xScale = width / chip8Width;
    const int yScale = height / chip8Height;

    for (int y = 0; y < chip8Height; y++)
    {
        for (int x = 0; x < chip8Width; x++)
        {
            const auto pixelColor = videoBuffer[y * chip8Width + x];
            XSetForeground(display, gc, pixelColor);
            XFillRectangle(display, window, gc, x * xScale, y * yScale, xScale, yScale);
        }
    }

    return true;
}

void platform_close_window()
{
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

int toggleKey(int keysym, bool pressed)
{
    switch (keysym)
    {
    case XK_1:
    {
        ToggleKey(KEY_CODE_1, pressed);
        break;
    }
    case XK_2:
    {
        ToggleKey(KEY_CODE_2, pressed);
        break;
    }
    case XK_3:
    {
        ToggleKey(KEY_CODE_3, pressed);
        break;
    }
    case XK_4:
    {
        ToggleKey(KEY_CODE_4, pressed);
        break;
    }
    case XK_q:
    case XK_Q:
    {
        ToggleKey(KEY_CODE_Q, pressed);
        break;
    }
    case XK_w:
    case XK_W:
    {
        ToggleKey(KEY_CODE_W, pressed);
        break;
    }
    case XK_e:
    case XK_E:
    {
        ToggleKey(KEY_CODE_E, pressed);
        break;
    }
    case XK_r:
    case XK_R:
    {
        ToggleKey(KEY_CODE_R, pressed);
        break;
    }
    case XK_a:
    case XK_A:
    {
        ToggleKey(KEY_CODE_A, pressed);
        break;
    }
    case XK_s:
    case XK_S:
    {
        ToggleKey(KEY_CODE_S, pressed);
        break;
    }
    case XK_d:
    case XK_D:
    {
        ToggleKey(KEY_CODE_D, pressed);
        break;
    }
    case XK_f:
    case XK_F:
    {
        ToggleKey(KEY_CODE_F, pressed);
        break;
    }
    case XK_z:
    case XK_Z:
    {
        ToggleKey(KEY_CODE_Z, pressed);
        break;
    }
    case XK_x:
    case XK_X:
    {
        ToggleKey(KEY_CODE_X, pressed);
        break;
    }

    case XK_c:
    case XK_C:
    {
        ToggleKey(KEY_CODE_C, pressed);
        break;
    }
    case XK_v:
    case XK_V:
    {
        ToggleKey(KEY_CODE_V, pressed);
        break;
    }
    }

    return -1;
}
