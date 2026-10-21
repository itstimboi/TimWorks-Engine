#include "sdlstuff.h"

#include <cstdio>
#include <cstring>

#include <glad/glad46.h>

#ifndef TWE_SDL_VERSION
#define TWE_SDL_VERSION 3
#endif

#if TWE_SDL_VERSION == 12
    #include <SDL/SDL.h>
#elif TWE_SDL_VERSION == 2
    #include <SDL2/SDL.h>
#elif TWE_SDL_VERSION == 3
    #include <SDL3/SDL.h>
#else
    #error "TWE_SDL_VERSION must be 12, 2, or 3"
#endif

namespace TWE
{
    static bool initialized = false;
    static bool quitRequested = false;

#if TWE_SDL_VERSION == 12

    struct WindowData
    {
        SDL_Surface* surface;
    };

#elif TWE_SDL_VERSION == 2 || TWE_SDL_VERSION == 3

    struct WindowData
    {
        SDL_Window* window;
        SDL_GLContext context;
    };

#endif

    SDLBackend GetSDLBackend()
    {
#if TWE_SDL_VERSION == 12
        return SDLBackend::SDL12;
#elif TWE_SDL_VERSION == 2
        return SDLBackend::SDL2;
#else
        return SDLBackend::SDL3;
#endif
    }

    bool InitializeSDLBackend()
    {
        if (initialized)
            return true;

#if TWE_SDL_VERSION == 12
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
#elif TWE_SDL_VERSION == 2
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
#else
        if (!SDL_Init(SDL_INIT_VIDEO))
#endif
        {
            std::fprintf(
                stderr,
                "TimWorks Engine: SDL initialization failed: %s\n",
                SDL_GetError()
            );

            return false;
        }

        initialized = true;
        quitRequested = false;
        return true;
    }

    void ShutdownSDLBackend()
    {
        if (!initialized)
            return;

        SDL_Quit();

        initialized = false;
        quitRequested = false;
    }

    bool SetSDLGLVersion(int major, int minor)
    {
        if (!initialized)
            return false;

#if TWE_SDL_VERSION == 12
        return SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_MAJOR_VERSION, major
        ) == 0
        &&
        SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_MINOR_VERSION, minor
        ) == 0;

#elif TWE_SDL_VERSION == 2 || TWE_SDL_VERSION == 3
        return SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_MAJOR_VERSION, major
        )
        &&
        SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_MINOR_VERSION, minor
        );
#endif
    }

    void* CreateSDLWindow(
        int width,
        int height,
        const char* title,
        bool fullscreen,
        bool visible
    )
    {
        if (!initialized || width <= 0 || height <= 0)
            return nullptr;

        if (!title)
            title = "TimWorks Engine Window";

        auto* data = new WindowData{};

#if TWE_SDL_VERSION == 12

        Uint32 flags = SDL_OPENGL;

        if (fullscreen)
            flags |= SDL_FULLSCREEN;

        if (!visible)
        {
            std::fprintf(
                stderr,
                "TimWorks Engine: SDL 1.2 does not provide "
                "a portable hidden-window equivalent.\n"
            );
        }

        data->surface = SDL_SetVideoMode(
            width, height, 0, flags
        );

        if (!data->surface)
        {
            std::fprintf(
                stderr,
                "TimWorks Engine: SDL 1.2 window failed: %s\n",
                SDL_GetError()
            );

            delete data;
            return nullptr;
        }

        SDL_WM_SetCaption(title, title);

#elif TWE_SDL_VERSION == 2

        Uint32 flags = SDL_WINDOW_OPENGL;

        flags |= visible
            ? SDL_WINDOW_SHOWN
            : SDL_WINDOW_HIDDEN;

        if (fullscreen)
            flags |= SDL_WINDOW_FULLSCREEN;

        data->window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            flags
        );

        if (!data->window)
        {
            std::fprintf(
                stderr,
                "TimWorks Engine: SDL2 window failed: %s\n",
                SDL_GetError()
            );

            delete data;
            return nullptr;
        }

        data->context = SDL_GL_CreateContext(data->window);

        if (!data->context)
        {
            std::fprintf(
                stderr,
                "TimWorks Engine: SDL2 OpenGL context failed: %s\n",
                SDL_GetError()
            );

            SDL_DestroyWindow(data->window);
            delete data;
            return nullptr;
        }

        SDL_GL_MakeCurrent(data->window, data->context);

#else

        SDL_WindowFlags flags = SDL_WINDOW_OPENGL;

        if (!visible)
            flags |= SDL_WINDOW_HIDDEN;

        if (fullscreen)
            flags |= SDL_WINDOW_FULLSCREEN;

        data->window = SDL_CreateWindow(
            title,
            width,
            height,
            flags
        );

        if (!data->window)
        {
            std::fprintf(
                stderr,
                "TimWorks Engine: SDL3 window failed: %s\n",
                SDL_GetError()
            );

            delete data;
            return nullptr;
        }

        data->context = SDL_GL_CreateContext(data->window);

        if (!data->context)
        {
            std::fprintf(
                stderr,
                "TimWorks Engine: SDL3 OpenGL context failed: %s\n",
                SDL_GetError()
            );

            SDL_DestroyWindow(data->window);
            delete data;
            return nullptr;
        }

        SDL_GL_MakeCurrent(data->window, data->context);

#endif

        return data;
    }

    void DestroySDLWindow(void* window)
    {
        if (!window)
            return;

        auto* data = static_cast<WindowData*>(window);

#if TWE_SDL_VERSION == 12

        // SDL 1.2 manages its video surface globally.
        // SDL_Quit() releases the video subsystem.

#else

        if (data->context)
            SDL_GL_DeleteContext(data->context);

        if (data->window)
            SDL_DestroyWindow(data->window);

#endif

        delete data;
    }

    void MakeSDLContextCurrent(void* window)
    {
        if (!window)
            return;

#if TWE_SDL_VERSION == 12

        // SDL 1.2 manages its OpenGL context globally.

#else

        auto* data = static_cast<WindowData*>(window);

        SDL_GL_MakeCurrent(data->window, data->context);

#endif
    }

    void SwapSDLBuffers(void* window)
    {
        if (!window)
            return;

#if TWE_SDL_VERSION == 12

        SDL_GL_SwapBuffers();

#elif TWE_SDL_VERSION == 2 || TWE_SDL_VERSION == 3

        auto* data = static_cast<WindowData*>(window);
        SDL_GL_SwapWindow(data->window);

#endif
    }

    void PollSDLEvents()
    {
#if TWE_SDL_VERSION == 12 || TWE_SDL_VERSION == 2

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quitRequested = true;
        }

#else

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                quitRequested = true;
        }

#endif
    }

    bool SDLWindowShouldClose(void* window)
    {
        return !window || quitRequested;
    }

    void SetSDLWindowTitle(void* window, const char* title)
    {
        if (!window || !title)
            return;

#if TWE_SDL_VERSION == 12

        SDL_WM_SetCaption(title, title);

#else

        auto* data = static_cast<WindowData*>(window);

#if TWE_SDL_VERSION == 2
        SDL_SetWindowTitle(data->window, title);
#else
        SDL_SetWindowTitle(data->window, title);
#endif

#endif
    }

    void SetSDLWindowSize(void* window, int width, int height)
    {
        if (!window || width <= 0 || height <= 0)
            return;

#if TWE_SDL_VERSION == 12

        // SDL 1.2 cannot resize its existing video surface
        // through the same API as SDL2/SDL3.
        (void)width;
        (void)height;

#else

        auto* data = static_cast<WindowData*>(window);
        SDL_SetWindowSize(data->window, width, height);

#endif
    }

    bool GetSDLWindowSize(
        void* window,
        int* width,
        int* height
    )
    {
        if (!window || !width || !height)
            return false;

#if TWE_SDL_VERSION == 12

        auto* data = static_cast<WindowData*>(window);

        *width = data->surface->w;
        *height = data->surface->h;

#else

        auto* data = static_cast<WindowData*>(window);

#if TWE_SDL_VERSION == 2
        SDL_GetWindowSize(data->window, width, height);
#else
        SDL_GetWindowSize(data->window, width, height);
#endif

#endif

        return true;
    }

    const char* GetGLVersionString()
    {
        const GLubyte* version = glGetString(GL_VERSION);

        return version
            ? reinterpret_cast<const char*>(version)
            : nullptr;
    }
}