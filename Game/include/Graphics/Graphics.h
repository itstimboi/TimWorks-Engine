#pragma once

namespace TWE
{

    // ============================================================
    // Graphics Window
    // ============================================================

    struct GraphicsWindow;


    // ============================================================
    // Renderer Types
    // ============================================================

    enum class Renderer
    {
        GL,
        DX,
        VK,
        Software,
        None
    };


    enum class GLRenderer
    {
        gl46,
        gl45,
        gl44,
        gl43,
        gl42,
        gl41,
        gl40,

        gl33,
        gl32,
        gl31,
        gl30,

        gl21,
        gl20,

        gl15,
        gl14,
        gl13,
        gl12,
        gl11,
        gl10,

        None
    };


    enum class DXRenderer
    {
        dx120,

        dx110,
        dx100,

        dx90c,
        dx90b,
        dx90a,
        dx90,

        dx81b,
        dx81,
        dx80,

        dx70a,
        dx70,

        dx61,
        dx60,

        dx52a,
        dx52,
        dx51,
        dx50,

        dx30a,
        dx30,

        dx20,

        dx10,

        None
    };


    enum class VKRenderer
    {
        vk14,
        vk13,

        None
    };


    struct GraphicsRenderer
    {
        Renderer API = Renderer::None;

        GLRenderer GL = GLRenderer::None;
        DXRenderer DX = DXRenderer::None;
        VKRenderer VK = VKRenderer::None;
    };


    // ============================================================
    // Detection
    // ============================================================

    GLRenderer bestGLVersion();

    GraphicsRenderer DetectBestRenderer();

    // ============================================================
    // Window Creation
    // ============================================================

    GraphicsWindow* CreateGraphicsWindow(
        int width,
        int height,
        const char* title
    );

    GraphicsWindow* CreateGraphicsWindow(
        int width,
        int height,
        const char* title,
        GraphicsRenderer renderer
    );


    // ============================================================
    // Window Destruction
    // ============================================================

    void DestroyGraphicsWindow(
        GraphicsWindow* window
    );


    // ============================================================
    // Renderer Switching
    // ============================================================

    bool SwitchRenderer(
        GraphicsWindow* window,
        GraphicsRenderer renderer
    );


    // ============================================================
    // Current Window
    // ============================================================

    void GraphicsMakeCurrent(
        GraphicsWindow* window
    );


    // ============================================================
    // Window Information
    // ============================================================

    int GraphicsGetWidth(
        GraphicsWindow* window
    );

    int GraphicsGetHeight(
        GraphicsWindow* window
    );


    // ============================================================
    // Renderer Information
    // ============================================================

    GraphicsRenderer GetGraphicsRenderer(
        GraphicsWindow* window
    );

    Renderer GetRenderer(
        GraphicsWindow* window
    );

    GLRenderer GetGLRenderer();

    DXRenderer GetDXRenderer(
        GraphicsWindow* window
    );

    VKRenderer GetVKRenderer(
        GraphicsWindow* window
    );


    // ============================================================
    // Renderer Names
    // ============================================================

    const char* RendererName(
        Renderer renderer
    );

    const char* GLRendererName(
        GLRenderer renderer
    );

    const char* DXRendererName(
        DXRenderer renderer
    );

    const char* VKRendererName(
        VKRenderer renderer
    );


    // ============================================================
    // Window Operations
    // ============================================================

    void GraphicsSwapBuffers(
        GraphicsWindow* window
    );

    bool GraphicsWindowShouldClose(
        GraphicsWindow* window
    );

    void GraphicsPollEvents();


    // ============================================================
    // Legacy OpenGL
    // ============================================================

    bool IsUsingLegacyOpenGL(
        GraphicsWindow* window
    );


    // ============================================================
    // Shutdown
    // ============================================================

    void DeInitGraphics();

}