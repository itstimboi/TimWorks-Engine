#include "Windows.h"

#include "GLFW3Window.h"

WindowBackend bestWindowBackend()
{
    return WindowBackend::GLFW3;
}

Window* CreateBestWindow()
{
    WindowBackend backend = bestWindowBackend();

    switch (backend)
    {
        case WindowBackend::GLFW3:
            return new GLFW3Window();

        default:
            return nullptr;
    }
}