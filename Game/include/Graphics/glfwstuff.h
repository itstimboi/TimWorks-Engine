#pragma once

namespace TWE
{
    enum class GLFWBackend
    {
        None,
        GLFW2,
        GLFW3
    };

    GLFWBackend GetGLFWBackend();

    bool InitializeGLFWBackend();
    void ShutdownGLFWBackend();

    void* CreateGLFWWindow(
        int width,
        int height,
        const char* title,
        bool fullscreen = false,
        bool visible = true
    );

    void DestroyGLFWWindow(void* window);

    void MakeGLFWContextCurrent(void* window);

    void SwapGLFWBuffers(void* window);

    bool GLFWWindowShouldClose(void* window);

    void PollGLFWEvents();

    bool SetGLFWOpenGLVersion(int major, int minor);

    const char* GetGLVersionString();
}