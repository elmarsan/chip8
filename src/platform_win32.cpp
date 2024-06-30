#include <windows.h>
#include <winuser.h>

#include <cstdint>
#include <cstdint>
#include <format>

#include "chip8.h"
#include "platform.h"

HWND window;

// Helper for handling errors of window api
void handleError(const std::string& msg);

LRESULT CALLBACK WndProc(HWND window,    // handle to window
                         UINT msg,       // message identifier
                         WPARAM wParam,  // first message parameter
                         LPARAM lParam   // second message parameter
)
{
    switch (msg)
    {
    case WM_CREATE:
        // Initialize the window.
        return 0;

    case WM_PAINT:
    {
        OutputDebugString("WM_PAINT\n");
        PAINTSTRUCT paint;

        HDC deviceContext = BeginPaint(window, &paint);
        {
            RECT rect;
            rect.left = 0;
            rect.top = 0;
            rect.right = 100;
            rect.bottom = 100;

            HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));  // RGB(0, 255, 0) is green
            FillRect(deviceContext, &rect, greenBrush);
            DeleteObject(greenBrush);
        } EndPaint(window, &paint);

        return 0;
    }
    case WM_SIZE:
    {
        OutputDebugString("WM_SIZE\n");
        return 0;
    }
    case WM_CLOSE:
    {
        OutputDebugString("WM_CLOSE\n");
        return 0;
    }
    case WM_DESTROY:
    {
        OutputDebugString("WM_DESTROY\n");
        /* PostQuitMessage(0); */
        return 0;
    }
    default:
        return DefWindowProc(window, msg, wParam, lParam);
    }
}

bool platform_create_window(const std::string& title, const int width, const int height)
{
    HMODULE hInstance = GetModuleHandle(0);
    if (!hInstance)
    {
        handleError("Unable to GetModuleHandle");
        return false;
    }

    WNDCLASSA wndClass = {};
    wndClass.style = CS_OWNDC | CS_SAVEBITS | CS_DROPSHADOW | CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.hInstance = hInstance;
    wndClass.lpszClassName = title.c_str();

    if (!RegisterClassA(&wndClass))
    {
        handleError("Unable to RegisterClassA");
        return false;
    }

    DWORD style = WS_VISIBLE | WS_BORDER | WS_TILEDWINDOW;

    window = CreateWindowA(wndClass.lpszClassName,  // lpClassName,
                           wndClass.lpszClassName,  // lpWindowName,
                           style,                   // dwStyle,
                           CW_USEDEFAULT,           // x,
                           CW_USEDEFAULT,           // y,
                           width,                   // nWidth,
                           height,                  // nHeight,
                           0,                       // hWndParent,
                           0,                       // hMenu,
                           hInstance,               // hInstance,
                           0                        // lpParam
    );

    if (!window)
    {
        handleError("Unable to CreateWindowA");
        return false;
    }

    return true;
}

bool platform_update_window(const uint32_t (&videoBuffer)[chip8Width * chip8Height])
{
    /* if (!UpdateWindow(window)) */
    /* { */
    /*     return false; */
    /* } */

    MSG msg;
    BOOL messageResult = GetMessage(&msg, 0, 0, 0);
    if (messageResult <= 0)
    {
        return true;
    }

    TranslateMessage(&msg);
    DispatchMessage(&msg);

    return true;
}

void platform_close_window() { return; }

void handleError(const std::string& prefix)
{
    DWORD code = GetLastError();
    if (code == ERROR_SUCCESS)
    {
        return;
    }

    LPSTR buffer = nullptr;
    DWORD message = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,  // dwFlags,
                                  0,                                                            // lpSource,
                                  code,                                                         // dwMessageId,
                                  0,                                                            // dwLanguageId,
                                  (LPSTR)&buffer,                                               // lpBuffer,
                                  0,                                                            // nSize,
                                  0                                                             // *Arguments
    );

    std::string msg(buffer);
    std::string fmtMsg = std::format("{}: {}\n", prefix, msg);

    LocalFree(buffer);

    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsole(hOutput, fmtMsg.c_str(), strlen(fmtMsg.c_str()), nullptr, 0);
}
