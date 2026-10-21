#include "Graphics.h"
#include "glfwstuff.h"

#include <iostream>
#include <string>
#include <vector>


namespace TWE
{

    // ============================================================
    // Internal State
    // ============================================================

    static std::vector<GraphicsWindow*> graphicsWindows;

    static GraphicsWindow* currentWindow = nullptr;

    static bool graphicsInitialized = false;


    // ============================================================
    // Graphics Window
    // ============================================================

    struct GraphicsWindow
    {
        void* backendWindow = nullptr;

        int width = 0;
        int height = 0;

        std::string title;

        GraphicsRenderer renderer;
        
        bool legacyOpenGL = false;
    };


    // ============================================================
    // Renderer Names
    // ============================================================

    const char* RendererName(
        Renderer renderer
    )
    {
        switch (renderer)
        {
            case Renderer::GL:
                return "OpenGL";

            case Renderer::DX:
                return "DirectX";

            case Renderer::VK:
                return "Vulkan";

            case Renderer::Software:
                return "Software";

            case Renderer::None:
                return "None";
        }

        return "Unknown";
    }


    const char* GLRendererName(
        GLRenderer renderer
    )
    {
        switch (renderer)
        {
            case GLRenderer::gl46: return "OpenGL 4.6";
            case GLRenderer::gl45: return "OpenGL 4.5";
            case GLRenderer::gl44: return "OpenGL 4.4";
            case GLRenderer::gl43: return "OpenGL 4.3";
            case GLRenderer::gl42: return "OpenGL 4.2";
            case GLRenderer::gl41: return "OpenGL 4.1";
            case GLRenderer::gl40: return "OpenGL 4.0";

            case GLRenderer::gl33: return "OpenGL 3.3";
            case GLRenderer::gl32: return "OpenGL 3.2";
            case GLRenderer::gl31: return "OpenGL 3.1";
            case GLRenderer::gl30: return "OpenGL 3.0";

            case GLRenderer::gl21: return "OpenGL 2.1";
            case GLRenderer::gl20: return "OpenGL 2.0";

            case GLRenderer::gl15: return "OpenGL 1.5";
            case GLRenderer::gl14: return "OpenGL 1.4";
            case GLRenderer::gl13: return "OpenGL 1.3";
            case GLRenderer::gl12: return "OpenGL 1.2";
            case GLRenderer::gl11: return "OpenGL 1.1";
            case GLRenderer::gl10: return "OpenGL 1.0";

            case GLRenderer::None:
                return "None";
        }

        return "Unknown";
    }


    const char* DXRendererName(
        DXRenderer renderer
    )
    {
        switch (renderer)
        {
            case DXRenderer::dx120: return "DirectX 12";
            case DXRenderer::dx110: return "DirectX 11";
            case DXRenderer::dx100: return "DirectX 10";

            case DXRenderer::dx90c: return "DirectX 9.0c";
            case DXRenderer::dx90b: return "DirectX 9.0b";
            case DXRenderer::dx90a: return "DirectX 9.0a";
            case DXRenderer::dx90:  return "DirectX 9";

            case DXRenderer::dx81b: return "DirectX 8.1b";
            case DXRenderer::dx81:  return "DirectX 8.1";
            case DXRenderer::dx80:  return "DirectX 8.0";

            case DXRenderer::dx70a: return "DirectX 7.0a";
            case DXRenderer::dx70:  return "DirectX 7.0";

            case DXRenderer::dx61: return "DirectX 6.1";
            case DXRenderer::dx60: return "DirectX 6.0";

            case DXRenderer::dx52a: return "DirectX 5.2a";
            case DXRenderer::dx52:  return "DirectX 5.2";
            case DXRenderer::dx51:  return "DirectX 5.1";
            case DXRenderer::dx50:  return "DirectX 5.0";

            case DXRenderer::dx30a: return "DirectX 3.0a";
            case DXRenderer::dx30:  return "DirectX 3.0";

            case DXRenderer::dx20: return "DirectX 2.0";
            case DXRenderer::dx10: return "DirectX 1.0";

            case DXRenderer::None:
                return "None";
        }

        return "Unknown";
    }


    const char* VKRendererName(
        VKRenderer renderer
    )
    {
        switch (renderer)
        {
            case VKRenderer::vk14:
                return "Vulkan 1.4";

            case VKRenderer::vk13:
                return "Vulkan 1.3";

            case VKRenderer::None:
                return "None";
        }

        return "Unknown";
    }


    // ============================================================
    // Initialization
    // ============================================================

    static bool InitializeGraphics()
    {
        if (graphicsInitialized)
            return true;

        graphicsInitialized =
            InitializeGLFWBackend();

        if (!graphicsInitialized)
        {
            std::cout
                << "Failed to initialize graphics backend.\n";

            return false;
        }

        return true;
    }


    // ============================================================
    // OpenGL Version
    // ============================================================

    GLRenderer bestGLVersion()
    {
        if (!InitializeGraphics())
            return GLRenderer::None;

        return GetGLRenderer();
    }


    // ============================================================
    // Renderer Detection
    // ============================================================

    GraphicsRenderer DetectBestRenderer()
    {
        GraphicsRenderer renderer{};

        // --------------------------------------------------------
        // Vulkan
        // --------------------------------------------------------

        // TODO:
        // Vulkan detection will go here.


        // --------------------------------------------------------
        // DirectX
        // --------------------------------------------------------

        // TODO:
        // DirectX detection will go here.


        // --------------------------------------------------------
        // OpenGL
        // --------------------------------------------------------

        GLRenderer gl =
            bestGLVersion();

        if (gl != GLRenderer::None)
        {
            renderer.API =
                Renderer::GL;

            renderer.GL =
                gl;

            renderer.DX =
                DXRenderer::None;

            renderer.VK =
                VKRenderer::None;

            return renderer;
        }


        // --------------------------------------------------------
        // Software
        // --------------------------------------------------------

        renderer.API =
            Renderer::Software;

        renderer.GL =
            GLRenderer::None;

        renderer.DX =
            DXRenderer::None;

        renderer.VK =
            VKRenderer::None;

        return renderer;
    }

    // ============================================================
    // Create Window
    // ============================================================

    GraphicsWindow* CreateGraphicsWindow(
        int width,
        int height,
        const char* title
    )
    {
        GraphicsRenderer renderer =
            DetectBestRenderer();

        return CreateGraphicsWindow(
            width,
            height,
            title,
            renderer
        );
    }


    GraphicsWindow* CreateGraphicsWindow(
        int width,
        int height,
        const char* title,
        GraphicsRenderer renderer
    )
    {
        if (!InitializeGraphics())
            return nullptr;

        if (!title)
            title = "TimEngine";


        GraphicsWindow* window =
            new GraphicsWindow();


        window->width =
            width;

        window->height =
            height;

        window->title =
            title;

        window->renderer =
            renderer;

        window->legacyOpenGL =
            false;


        // --------------------------------------------------------
        // OpenGL
        // --------------------------------------------------------

        if (renderer.API == Renderer::GL)
        {
            if (renderer.GL == GLRenderer::None)
            {
                delete window;
                return nullptr;
            }


            // ----------------------------------------------------
            // Version conversion
            // ----------------------------------------------------

            int major = 0;
            int minor = 0;

            switch (renderer.GL)
            {
                case GLRenderer::gl46:
                    major = 4; minor = 6;
                    break;

                case GLRenderer::gl45:
                    major = 4; minor = 5;
                    break;

                case GLRenderer::gl44:
                    major = 4; minor = 4;
                    break;

                case GLRenderer::gl43:
                    major = 4; minor = 3;
                    break;

                case GLRenderer::gl42:
                    major = 4; minor = 2;
                    break;

                case GLRenderer::gl41:
                    major = 4; minor = 1;
                    break;

                case GLRenderer::gl40:
                    major = 4; minor = 0;
                    break;

                case GLRenderer::gl33:
                    major = 3; minor = 3;
                    break;

                case GLRenderer::gl32:
                    major = 3; minor = 2;
                    break;

                case GLRenderer::gl31:
                    major = 3; minor = 1;
                    break;

                case GLRenderer::gl30:
                    major = 3; minor = 0;
                    break;

                case GLRenderer::gl21:
                    major = 2; minor = 1;
                    break;

                case GLRenderer::gl20:
                    major = 2; minor = 0;
                    break;

                case GLRenderer::gl15:
                    major = 1; minor = 5;
                    break;

                case GLRenderer::gl14:
                    major = 1; minor = 4;
                    break;

                case GLRenderer::gl13:
                    major = 1; minor = 3;
                    break;

                case GLRenderer::gl12:
                    major = 1; minor = 2;
                    break;

                case GLRenderer::gl11:
                    major = 1; minor = 1;
                    break;

                case GLRenderer::gl10:
                    major = 1; minor = 0;
                    break;

                case GLRenderer::None:
                    break;
            }


            if (major == 0)
            {
                delete window;
                return nullptr;
            }


            // ----------------------------------------------------
            // Backend creates the actual window/context.
            // ----------------------------------------------------

            window->backendWindow =
                CreateGLFWWindow(
                    width,
                    height,
                    title
                );


            if (!window->backendWindow)
            {
                delete window;
                return nullptr;
            }


            window->legacyOpenGL =
                major == 1;


            std::cout
                << "Created "
                << GLRendererName(renderer.GL)
                << " window.\n";


            const char* actualVersion =
                GetGLVersionString();


            if (actualVersion)
            {
                std::cout
                    << "Actual OpenGL: "
                    << actualVersion
                    << '\n';
            }
        }


        // --------------------------------------------------------
        // DirectX
        // --------------------------------------------------------

        else if (renderer.API == Renderer::DX)
        {
            std::cout
                << "DirectX renderer is not implemented yet.\n";

            delete window;
            return nullptr;
        }


        // --------------------------------------------------------
        // Vulkan
        // --------------------------------------------------------

        else if (renderer.API == Renderer::VK)
        {
            std::cout
                << "Vulkan renderer is not implemented yet.\n";

            delete window;
            return nullptr;
        }


        // --------------------------------------------------------
        // Software
        // --------------------------------------------------------

        else if (renderer.API == Renderer::Software)
        {
            std::cout
                << "Software renderer is not implemented yet.\n";

            delete window;
            return nullptr;
        }


        else
        {
            delete window;
            return nullptr;
        }


        graphicsWindows.push_back(
            window
        );

        currentWindow =
            window;

        return window;
    }


    // ============================================================
    // Destroy Window
    // ============================================================

    void DestroyGraphicsWindow(
        GraphicsWindow* window
    )
    {
        if (!window)
            return;


        if (currentWindow == window)
        {
            MakeGLFWContextCurrent(
                nullptr
            );

            currentWindow =
                nullptr;
        }


        if (window->backendWindow)
        {
            DestroyGLFWWindow(
                window->backendWindow
            );

            window->backendWindow =
                nullptr;
        }


        for (auto it =
            graphicsWindows.begin();
            it != graphicsWindows.end();
            ++it)
        {
            if (*it == window)
            {
                graphicsWindows.erase(it);
                break;
            }
        }


        if (currentWindow == nullptr &&
            !graphicsWindows.empty())
        {
            currentWindow =
                graphicsWindows.front();
        }


        delete window;
    }


    // ============================================================
    // Renderer Switching
    // ============================================================

    bool SwitchRenderer(
        GraphicsWindow* window,
        GraphicsRenderer renderer
    )
    {
        if (!window)
            return false;

        /*
            Renderer switching needs a new backend context.

            We intentionally do not destroy the supplied pointer
            here because GraphicsWindow is owned by the caller.

            This will be implemented once the renderer backend
            interface exists.
        */

        (void)renderer;

        return false;
    }


    // ============================================================
    // Make Current
    // ============================================================

    void GraphicsMakeCurrent(
        GraphicsWindow* window
    )
    {
        if (!window)
            return;

        currentWindow =
            window;

        MakeGLFWContextCurrent(
            window->backendWindow
        );
    }


    // ============================================================
    // Window Information
    // ============================================================

    int GraphicsGetWidth(
        GraphicsWindow* window
    )
    {
        if (!window)
            return 0;

        return window->width;
    }


    int GraphicsGetHeight(
        GraphicsWindow* window
    )
    {
        if (!window)
            return 0;

        return window->height;
    }


    // ============================================================
    // Renderer Information
    // ============================================================

    GraphicsRenderer GetGraphicsRenderer()
    {
        // if (!currentWindow)
        // {
        //     return;
        // }

        return currentWindow->renderer;
    }


    Renderer GetRenderer()
    {
        if (!currentWindow)
            return Renderer::None;

        return currentWindow->renderer.API;
    }

    static bool ParseGLVersion(
        const char* versionString,
        int& major,
        int& minor
    )
    {
        if (!versionString)
            return false;

        major = 0;
        minor = 0;

        return std::sscanf(
            versionString,
            "%d.%d",
            &major,
            &minor
        ) == 2;
    }

    GLRenderer GetGLRenderer()
    {
        const char* versionString =
            GetGLVersionString();

        if (!versionString)
            return GLRenderer::None;

        int major;
        int minor;

        if (!ParseGLVersion(versionString, major, minor))
            return GLRenderer::None;

        if (major > 4 || (major == 4 && minor >= 6))
            return GLRenderer::gl46;

        if (major == 4 && minor >= 5)
            return GLRenderer::gl45;

        if (major == 4 && minor >= 4)
            return GLRenderer::gl44;

        if (major == 4 && minor >= 3)
            return GLRenderer::gl43;

        if (major == 4 && minor >= 2)
            return GLRenderer::gl42;

        if (major == 4 && minor >= 1)
            return GLRenderer::gl41;

        if (major == 4 && minor >= 0)
            return GLRenderer::gl40;

        if (major == 3 && minor >= 3)
            return GLRenderer::gl33;

        if (major == 3 && minor >= 2)
            return GLRenderer::gl32;

        if (major == 3 && minor >= 1)
            return GLRenderer::gl31;

        if (major == 3 && minor >= 0)
            return GLRenderer::gl30;

        if (major == 2 && minor >= 1)
            return GLRenderer::gl21;

        if (major == 2 && minor >= 0)
            return GLRenderer::gl20;

        if (major == 1 && minor >= 5)
            return GLRenderer::gl15;

        if (major == 1 && minor >= 4)
            return GLRenderer::gl14;

        if (major == 1 && minor >= 3)
            return GLRenderer::gl13;

        if (major == 1 && minor >= 2)
            return GLRenderer::gl12;

        if (major == 1 && minor >= 1)
            return GLRenderer::gl11;

        if (major == 1 && minor >= 0)
            return GLRenderer::gl10;

        return GLRenderer::None;
    }

    DXRenderer GetDXRenderer()
    {
        if (!currentWindow)
            return DXRenderer::None;

        return currentWindow->renderer.DX;
    }


    VKRenderer GetVKRenderer()
    {
        if (!currentWindow)
            return VKRenderer::None;

        return currentWindow->renderer.VK;
    }


    // ============================================================
    // Legacy OpenGL
    // ============================================================

    bool IsUsingLegacyOpenGL()
    {
        if (!currentWindow)
            return false;

        return currentWindow->legacyOpenGL;
    }


    // ============================================================
    // Swap Buffers
    // ============================================================

    void GraphicsSwapBuffers(
        GraphicsWindow* window
    )
    {
        if (!window)
            return;

        if (window->renderer.API ==
            Renderer::GL)
        {
            SwapGLFWBuffers(
                window->backendWindow
            );
        }
    }


    // ============================================================
    // Window Close
    // ============================================================

    bool GraphicsWindowShouldClose(
        GraphicsWindow* window
    )
    {
        if (!window)
            return true;

        return GLFWWindowShouldClose(
            window->backendWindow
        );
    }


    // ============================================================
    // Events
    // ============================================================

    void GraphicsPollEvents()
    {
        if (!graphicsInitialized)
            return;

        PollGLFWEvents();
    }


    // ============================================================
    // Shutdown
    // ============================================================

    void DeInitGraphics()
    {
        while (!graphicsWindows.empty())
        {
            DestroyGraphicsWindow(
                graphicsWindows.back()
            );
        }


        currentWindow =
            nullptr;


        if (graphicsInitialized)
        {
            ShutdownGLFWBackend();

            graphicsInitialized =
                false;
        }
    }

}