#pragma once

namespace TE
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
        int glMajor,
        int glMinor,
        bool visible
    );

    void DestroyGLFWWindow(void* window);

    void MakeGLFWContextCurrent(void* window);

    void SwapGLFWBuffers(void* window);

    bool GLFWWindowShouldClose(void* window);

    void PollGLFWEvents();

    bool SetGLFWOpenGLVersion(int major, int minor);

    const char* GetGLVersionString();
}