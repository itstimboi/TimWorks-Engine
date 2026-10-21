#include "GLFW3Window.h"

GLFW3Window::GLFW3Window()
{
}

GLFW3Window::~GLFW3Window()
{
    Destroy();
}

bool GLFW3Window::Create(
    int width,
    int height,
    const char* title
)
{
    if (!glfwInit())
        return false;

    window = glfwCreateWindow(
        width,
        height,
        title,
        nullptr,
        nullptr
    );

    if (!window)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    return true;
}

void GLFW3Window::Destroy()
{
    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}

void GLFW3Window::PollEvents()
{
    glfwPollEvents();
}

bool GLFW3Window::ShouldClose() const
{
    if (!window)
        return true;

    return glfwWindowShouldClose(window);
}

void GLFW3Window::SwapBuffers()
{
    if (window)
        glfwSwapBuffers(window);
}

void* GLFW3Window::GetNativeHandle()
{
    return window;
}