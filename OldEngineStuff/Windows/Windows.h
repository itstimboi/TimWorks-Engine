#pragma once

enum class WindowBackend
{
    GLFW3,
    Win32,
    X11,
    Wayland,
    Cocoa,
    None
};

class Window
{
public:
    virtual ~Window() = default;

    virtual bool Create(
        int width,
        int height,
        const char* title
    ) = 0;

    virtual void Destroy() = 0;

    virtual void PollEvents() = 0;

    virtual bool ShouldClose() const = 0;

    virtual void SwapBuffers() = 0;

    virtual void* GetNativeHandle() = 0;
};

WindowBackend bestWindowBackend();

Window* CreateBestWindow();