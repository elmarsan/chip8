#include <windows.h>

#include <cstdint>
#include <chrono>
#include <format>
#include <string>
#include <thread>

#include "chip8.h"

int width = 800;
int height = 600;

Chip8 chip8{};
constexpr auto execPerTick = 12;

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

        OutputDebugString(std::format("Address of chip8: {}\n", static_cast<void*>(&chip8)).c_str());

        HDC deviceContext = BeginPaint(window, &paint);
        {
            const auto xScale = width / chip8Width;
            const auto yScale = height / chip8Height;

            for (int y = 0; y < chip8Height; y++)
            {
                for (int x = 0; x < chip8Width; x++)
                {
                    const auto pixelColor = chip8.videoBuffer[y * chip8Width + x];
                    RECT rect;
                    rect.left = x * xScale;
                    rect.top = y * yScale;
                    rect.right = rect.left + xScale;
                    rect.bottom = rect.top + yScale;

                    const auto r = (pixelColor >> 16) & 0xff;
                    const auto g = (pixelColor >> 8) & 0xff;
                    const auto b = pixelColor & 0xff;
                    HBRUSH brush = CreateSolidBrush(RGB(r, g, b));

                    FillRect(deviceContext, &rect, brush);
                    DeleteObject(brush);
                }
            }
        }
        EndPaint(window, &paint);
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

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    OutputDebugString(std::format("Address of chip8: {}\n", static_cast<void*>(&chip8)).c_str());
    int numArgs;
    LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &numArgs);
    if (args == nullptr)
    {
        MessageBox(NULL, "Missing program arguments", "Error", MB_OK);
        return 1;
    }

    if (numArgs < 2)
    {
        MessageBox(NULL, "Missing rom file argument", "Error", MB_OK);
        return 1;
    }

    std::wstring romPathArg = args[1];
    std::string romPath(romPathArg.begin(), romPathArg.end());
    OutputDebugStringA(romPath.c_str());

    if (!chip8.LoadRom(romPath))
    {
        std::string errMsg = std::format("Rom {} not found\n", romPath);
        MessageBox(NULL, errMsg.c_str(), "Error", MB_OK);
        return 1;
    }
    LocalFree(args);

    WNDCLASSA wndClass = {};
    wndClass.style = CS_OWNDC | CS_SAVEBITS | CS_DROPSHADOW | CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.hInstance = hInstance;
    wndClass.lpszClassName = "Chip8 windows";

    if (!RegisterClassA(&wndClass))
    {
        handleError("Unable to RegisterClassA");
        return 1;
    }

    HWND window = CreateWindowA(wndClass.lpszClassName,                   // lpClassName,
                                wndClass.lpszClassName,                   // lpWindowName,
                                WS_VISIBLE | WS_BORDER | WS_TILEDWINDOW,  // dwStyle,
                                CW_USEDEFAULT,                            // x,
                                CW_USEDEFAULT,                            // y,
                                width,                                    // nWidth,
                                height,                                   // nHeight,
                                0,                                        // hWndParent,
                                0,                                        // hMenu,
                                hInstance,                                // hInstance,
                                0                                         // lpParam
    );
    if (!window)
    {
        handleError("Unable to CreateWindowA");
        return 1;
    }

    MSG msg;

    const auto fps = 60;
    const auto frameDelay = 1000 / fps;

    do
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            auto frameStart = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < execPerTick; i++)
            {
                chip8.ExecuteNext();
            }

            InvalidateRect(window, NULL, true);

            std::chrono::duration<float, std::milli> frameDuration =
                std::chrono::high_resolution_clock::now() - frameStart;
            auto frameTime = frameDuration.count();

            if (frameDelay > frameTime)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(frameDelay - frameTime)));
            }
        }
    } while (msg.message != WM_QUIT);

    DestroyWindow(window);

    return 0;
}

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
