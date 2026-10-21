#ifndef SDLSTUFF_H
#define SDLSTUFF_H

namespace TWE
{
    enum class SDLBackend
    {
        None,
        SDL12,
        SDL2,
        SDL3
    };

    SDLBackend GetSDLBackend();

    bool InitializeSDLBackend();
    void ShutdownSDLBackend();

    bool SetSDLGLVersion(int major, int minor);

    void* CreateSDLWindow(
        int width,
        int height,
        const char* title,
        bool fullscreen = false,
        bool visible = true
    );

    void DestroySDLWindow(void* window);
    void MakeSDLContextCurrent(void* window);
    void SwapSDLBuffers(void* window);

    bool SDLWindowShouldClose(void* window);
    void PollSDLEvents();

    void SetSDLWindowTitle(
        void* window,
        const char* title
    );

    void SetSDLWindowSize(
        void* window,
        int width,
        int height
    );

    bool GetSDLWindowSize(
        void* window,
        int* width,
        int* height
    );

    const char* GetGLVersionString();
}

#endif