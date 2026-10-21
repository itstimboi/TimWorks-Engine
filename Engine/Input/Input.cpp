// #include "Input.h"

// #include <cstring>

// namespace TWE
// {
//     namespace Input
//     {
//         static GLFWwindow* g_Window = nullptr;

//         static GLFWgamepadstate g_CurrentGamepadState[GLFW_JOYSTICK_LAST + 1]{};
//         static GLFWgamepadstate g_PreviousGamepadState[GLFW_JOYSTICK_LAST + 1]{};

//         static bool g_GamepadValid[GLFW_JOYSTICK_LAST + 1]{};
//         static bool g_PreviousGamepadValid[GLFW_JOYSTICK_LAST + 1]{};


//         // ============================================================
//         // Initialization
//         // ============================================================

//         void Init(GLFWwindow* window)
//         {
//             g_Window = window;

//             std::memset(
//                 g_CurrentGamepadState,
//                 0,
//                 sizeof(g_CurrentGamepadState)
//             );

//             std::memset(
//                 g_PreviousGamepadState,
//                 0,
//                 sizeof(g_PreviousGamepadState)
//             );

//             std::memset(
//                 g_GamepadValid,
//                 0,
//                 sizeof(g_GamepadValid)
//             );

//             std::memset(
//                 g_PreviousGamepadValid,
//                 0,
//                 sizeof(g_PreviousGamepadValid)
//             );
//         }


//         // ============================================================
//         // Update
//         // ============================================================

//         void Update()
//         {
//             if (!g_Window)
//                 return;


//             // --------------------------------------------------------
//             // Save previous gamepad state
//             // --------------------------------------------------------

//             for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i)
//             {
//                 g_PreviousGamepadState[i] =
//                     g_CurrentGamepadState[i];

//                 g_PreviousGamepadValid[i] =
//                     g_GamepadValid[i];
//             }


//             // --------------------------------------------------------
//             // Get current gamepad states
//             // --------------------------------------------------------

//             for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i)
//             {
//                 g_GamepadValid[i] =
//                     glfwGetGamepadState(
//                         i,
//                         &g_CurrentGamepadState[i]
//                     );
//             }
//         }


//         // ============================================================
//         // Keyboard
//         // ============================================================

//         bool IsKeyDown(Key key)
//         {
//             if (!g_Window)
//                 return false;

//             return glfwGetKey(
//                 g_Window,
//                 static_cast<int>(key)
//             ) == GLFW_PRESS;
//         }


//         bool IsKeyPressed(Key key)
//         {
//             if (!g_Window)
//                 return false;

//             static bool previous[GLFW_KEY_LAST + 1]{};

//             int keyCode = static_cast<int>(key);

//             if (keyCode < 0 || keyCode > GLFW_KEY_LAST)
//                 return false;

//             bool current =
//                 glfwGetKey(
//                     g_Window,
//                     keyCode
//                 ) == GLFW_PRESS;

//             bool pressed =
//                 current && !previous[keyCode];

//             previous[keyCode] = current;

//             return pressed;
//         }


//         bool IsKeyReleased(Key key)
//         {
//             if (!g_Window)
//                 return false;

//             static bool previous[GLFW_KEY_LAST + 1]{};

//             int keyCode = static_cast<int>(key);

//             if (keyCode < 0 || keyCode > GLFW_KEY_LAST)
//                 return false;

//             bool current =
//                 glfwGetKey(
//                     g_Window,
//                     keyCode
//                 ) == GLFW_PRESS;

//             bool released =
//                 !current && previous[keyCode];

//             previous[keyCode] = current;

//             return released;
//         }


//         // ============================================================
//         // Mouse
//         // ============================================================

//         bool IsMouseButtonDown(MouseButton button)
//         {
//             if (!g_Window)
//                 return false;

//             return glfwGetMouseButton(
//                 g_Window,
//                 static_cast<int>(button)
//             ) == GLFW_PRESS;
//         }


//         bool IsMouseButtonPressed(MouseButton button)
//         {
//             static bool previous[GLFW_MOUSE_BUTTON_LAST + 1]{};

//             int buttonCode = static_cast<int>(button);

//             if (buttonCode < 0 ||
//                 buttonCode > GLFW_MOUSE_BUTTON_LAST)
//             {
//                 return false;
//             }

//             bool current =
//                 IsMouseButtonDown(button);

//             bool pressed =
//                 current && !previous[buttonCode];

//             previous[buttonCode] = current;

//             return pressed;
//         }


//         bool IsMouseButtonReleased(MouseButton button)
//         {
//             static bool previous[GLFW_MOUSE_BUTTON_LAST + 1]{};

//             int buttonCode = static_cast<int>(button);

//             if (buttonCode < 0 ||
//                 buttonCode > GLFW_MOUSE_BUTTON_LAST)
//             {
//                 return false;
//             }

//             bool current =
//                 IsMouseButtonDown(button);

//             bool released =
//                 !current && previous[buttonCode];

//             previous[buttonCode] = current;

//             return released;
//         }


//         double GetMouseX()
//         {
//             if (!g_Window)
//                 return 0.0;

//             double x;
//             double y;

//             glfwGetCursorPos(
//                 g_Window,
//                 &x,
//                 &y
//             );

//             return x;
//         }


//         double GetMouseY()
//         {
//             if (!g_Window)
//                 return 0.0;

//             double x;
//             double y;

//             glfwGetCursorPos(
//                 g_Window,
//                 &x,
//                 &y
//             );

//             return y;
//         }


//         void GetMousePosition(double& x, double& y)
//         {
//             if (!g_Window)
//             {
//                 x = 0.0;
//                 y = 0.0;
//                 return;
//             }

//             glfwGetCursorPos(
//                 g_Window,
//                 &x,
//                 &y
//             );
//         }


//         // ============================================================
//         // Gamepad
//         // ============================================================

//         bool IsGamepadConnected(int gamepad)
//         {
//             if (gamepad < 0 ||
//                 gamepad > GLFW_JOYSTICK_LAST)
//             {
//                 return false;
//             }

//             return glfwJoystickPresent(gamepad) &&
//                 glfwJoystickIsGamepad(gamepad);
//         }


//         bool IsGamepadDown(
//             GamepadButton button,
//             int gamepad
//         )
//         {
//             if (!IsGamepadConnected(gamepad))
//                 return false;

//             int buttonCode =
//                 static_cast<int>(button);

//             if (buttonCode < 0 ||
//                 buttonCode > GLFW_GAMEPAD_BUTTON_LAST)
//             {
//                 return false;
//             }

//             return
//                 g_CurrentGamepadState[gamepad]
//                     .buttons[buttonCode]
//                 == GLFW_PRESS;
//         }


//         bool IsGamepadPressed(
//             GamepadButton button,
//             int gamepad
//         )
//         {
//             if (!IsGamepadConnected(gamepad))
//                 return false;

//             int buttonCode =
//                 static_cast<int>(button);

//             if (buttonCode < 0 ||
//                 buttonCode > GLFW_GAMEPAD_BUTTON_LAST)
//             {
//                 return false;
//             }

//             bool current =
//                 g_CurrentGamepadState[gamepad]
//                     .buttons[buttonCode]
//                 == GLFW_PRESS;

//             bool previous =
//                 g_PreviousGamepadState[gamepad]
//                     .buttons[buttonCode]
//                 == GLFW_PRESS;

//             return current && !previous;
//         }


//         bool IsGamepadReleased(
//             GamepadButton button,
//             int gamepad
//         )
//         {
//             if (!IsGamepadConnected(gamepad))
//                 return false;

//             int buttonCode =
//                 static_cast<int>(button);

//             if (buttonCode < 0 ||
//                 buttonCode > GLFW_GAMEPAD_BUTTON_LAST)
//             {
//                 return false;
//             }

//             bool current =
//                 g_CurrentGamepadState[gamepad]
//                     .buttons[buttonCode]
//                 == GLFW_PRESS;

//             bool previous =
//                 g_PreviousGamepadState[gamepad]
//                     .buttons[buttonCode]
//                 == GLFW_PRESS;

//             return !current && previous;
//         }


//         float GetGamepadAxis(
//             GamepadAxis axis,
//             int gamepad
//         )
//         {
//             if (!IsGamepadConnected(gamepad))
//                 return 0.0f;

//             int axisCode =
//                 static_cast<int>(axis);

//             if (axisCode < 0 ||
//                 axisCode > GLFW_GAMEPAD_AXIS_LAST)
//             {
//                 return 0.0f;
//             }

//             return
//                 g_CurrentGamepadState[gamepad]
//                     .axes[axisCode];
//         }


//         const char* GetGamepadName(int gamepad)
//         {
//             if (!IsGamepadConnected(gamepad))
//                 return nullptr;

//             return glfwGetGamepadName(gamepad);
//         }
//     }    
// }

#include "Input.h"
#include <cstring>

namespace TWE {
namespace Input {

// ============================================================
// Backend window handle
// ============================================================

#if defined(__MINGW64__)

static GLFWwindow* g_Window = nullptr;

#elif defined(__MINGW32__) && !defined(__MINGW64__)

static void* g_Window = nullptr;

#else

#error "TimWorks Engine: Unsupported MinGW target."

#endif


// ============================================================
// Gamepad state
// GLFW3 has built-in gamepad support.
// GLFW2 does not, so the GLFW2 implementation below is
// intentionally a no-op until a legacy joystick layer is added.
// ============================================================

#if defined(__MINGW64__)

static GLFWgamepadstate g_CurrentGamepadState[
    GLFW_JOYSTICK_LAST + 1
]{};

static GLFWgamepadstate g_PreviousGamepadState[
    GLFW_JOYSTICK_LAST + 1
]{};

static bool g_GamepadValid[
    GLFW_JOYSTICK_LAST + 1
]{};

static bool g_PreviousGamepadValid[
    GLFW_JOYSTICK_LAST + 1
]{};

#endif


// ============================================================
// Initialization
// ============================================================

void Init(
#if defined(__MINGW64__)
    void* window
#elif defined(__MINGW32__) && !defined(__MINGW64__)
    void* window
#endif
)
{
#if defined(__MINGW64__)

    g_Window = static_cast<GLFWwindow*>(window);

    std::memset(
        g_CurrentGamepadState,
        0,
        sizeof(g_CurrentGamepadState)
    );

    std::memset(
        g_PreviousGamepadState,
        0,
        sizeof(g_PreviousGamepadState)
    );

    std::memset(
        g_GamepadValid,
        0,
        sizeof(g_GamepadValid)
    );

    std::memset(
        g_PreviousGamepadValid,
        0,
        sizeof(g_PreviousGamepadValid)
    );

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    // GLFW2 uses a global window.
    // Keep the parameter for API compatibility.
    g_Window = window;

#endif
}


// ============================================================
// Update
// ============================================================

void Update()
{
#if defined(__MINGW64__)

    // Save previous gamepad state.
    for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i)
    {
        g_PreviousGamepadState[i] = g_CurrentGamepadState[i];
        g_PreviousGamepadValid[i] = g_GamepadValid[i];

        g_GamepadValid[i] = false;

        if (glfwJoystickPresent(i) &&
            glfwJoystickIsGamepad(i))
        {
            if (glfwGetGamepadState(
                    i,
                    &g_CurrentGamepadState[i]))
            {
                g_GamepadValid[i] = true;
            }
        }
    }

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    // GLFW2 has no GLFW3-style gamepad API.
    // Keyboard and mouse state are queried directly,
    // so there is nothing to update here.

#endif
}


// ============================================================
// Keyboard
// ============================================================

bool IsKeyDown(Key key)
{
#if defined(__MINGW64__)

    if (!g_Window)
        return false;

    return glfwGetKey(
        g_Window,
        static_cast<int>(key)
    ) == GLFW_PRESS;

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    return glfw2GetKey(
        static_cast<int>(key)
    ) == GLFW2_PRESS;

#endif
}


bool IsKeyPressed(Key key)
{
#if defined(__MINGW64__)

    if (!g_Window)
        return false;

    static bool previous[
        GLFW_KEY_LAST + 1
    ]{};

    int k = static_cast<int>(key);

    if (k < 0 || k > GLFW_KEY_LAST)
        return false;

    bool current =
        glfwGetKey(g_Window, k) == GLFW_PRESS;

    bool pressed =
        current && !previous[k];

    previous[k] = current;

    return pressed;

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    static bool previous[512]{};

    int k = static_cast<int>(key);

    if (k < 0 || k >= 512)
        return false;

    bool current =
        glfw2GetKey(k) == GLFW2_PRESS;

    bool pressed =
        current && !previous[k];

    previous[k] = current;

    return pressed;

#endif
}


bool IsKeyReleased(Key key)
{
#if defined(__MINGW64__)

    if (!g_Window)
        return false;

    static bool previous[
        GLFW_KEY_LAST + 1
    ]{};

    int k = static_cast<int>(key);

    if (k < 0 || k > GLFW_KEY_LAST)
        return false;

    bool current =
        glfwGetKey(g_Window, k) == GLFW_PRESS;

    bool released =
        !current && previous[k];

    previous[k] = current;

    return released;

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    static bool previous[512]{};

    int k = static_cast<int>(key);

    if (k < 0 || k >= 512)
        return false;

    bool current =
        glfw2GetKey(k) == GLFW2_PRESS;

    bool released =
        !current && previous[k];

    previous[k] = current;

    return released;

#endif
}


// ============================================================
// Mouse
// ============================================================

bool IsMouseButtonDown(MouseButton button)
{
#if defined(__MINGW64__)

    if (!g_Window)
        return false;

    return glfwGetMouseButton(
        g_Window,
        static_cast<int>(button)
    ) == GLFW_PRESS;

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    return glfw2GetMouseButton(
        static_cast<int>(button)
    ) == GLFW2_PRESS;

#endif
}


bool IsMouseButtonPressed(MouseButton button)
{
#if defined(__MINGW64__)

    static bool previous[
        GLFW_MOUSE_BUTTON_LAST + 1
    ]{};

    int b = static_cast<int>(button);

    if (b < 0 || b > GLFW_MOUSE_BUTTON_LAST)
        return false;

    bool current = IsMouseButtonDown(button);

    bool pressed =
        current && !previous[b];

    previous[b] = current;

    return pressed;

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    static bool previous[16]{};

    int b = static_cast<int>(button);

    if (b < 0 || b >= 16)
        return false;

    bool current = IsMouseButtonDown(button);

    bool pressed =
        current && !previous[b];

    previous[b] = current;

    return pressed;

#endif
}


bool IsMouseButtonReleased(MouseButton button)
{
#if defined(__MINGW64__)

    static bool previous[
        GLFW_MOUSE_BUTTON_LAST + 1
    ]{};

    int b = static_cast<int>(button);

    if (b < 0 || b > GLFW_MOUSE_BUTTON_LAST)
        return false;

    bool current = IsMouseButtonDown(button);

    bool released =
        !current && previous[b];

    previous[b] = current;

    return released;

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    static bool previous[16]{};

    int b = static_cast<int>(button);

    if (b < 0 || b >= 16)
        return false;

    bool current = IsMouseButtonDown(button);

    bool released =
        !current && previous[b];

    previous[b] = current;

    return released;

#endif
}


double GetMouseX()
{
#if defined(__MINGW64__)

    if (!g_Window)
        return 0.0;

    double x = 0.0;
    double y = 0.0;

    glfwGetCursorPos(
        g_Window,
        &x,
        &y
    );

    return x;

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    int x = 0;
    int y = 0;

    glfw2GetMousePos(
        &x,
        &y
    );

    return static_cast<double>(x);

#endif
}


double GetMouseY()
{
#if defined(__MINGW64__)

    if (!g_Window)
        return 0.0;

    double x = 0.0;
    double y = 0.0;

    glfwGetCursorPos(
        g_Window,
        &x,
        &y
    );

    return y;

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    int x = 0;
    int y = 0;

    glfw2GetMousePos(
        &x,
        &y
    );

    return static_cast<double>(y);

#endif
}


void GetMousePosition(
    double& x,
    double& y
)
{
#if defined(__MINGW64__)

    if (!g_Window)
    {
        x = 0.0;
        y = 0.0;
        return;
    }

    glfwGetCursorPos(
        g_Window,
        &x,
        &y
    );

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    int ix = 0;
    int iy = 0;

    glfw2GetMousePos(
        &ix,
        &iy
    );

    x = static_cast<double>(ix);
    y = static_cast<double>(iy);

#endif
}


// ============================================================
// Gamepad
// ============================================================

bool IsGamepadConnected(int gamepad)
{
#if defined(__MINGW64__)

    if (gamepad < 0 ||
        gamepad > GLFW_JOYSTICK_LAST)
        return false;

    return glfwJoystickPresent(gamepad) &&
           glfwJoystickIsGamepad(gamepad);

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    // GLFW2 does not provide GLFW3's gamepad API.
    (void)gamepad;
    return false;

#endif
}


bool IsGamepadDown(
    GamepadButton button,
    int gamepad
)
{
#if defined(__MINGW64__)

    if (gamepad < 0 ||
        gamepad > GLFW_JOYSTICK_LAST)
        return false;

    if (!g_GamepadValid[gamepad])
        return false;

    int b = static_cast<int>(button);

    if (b < 0 ||
        b > GLFW_GAMEPAD_BUTTON_LAST)
        return false;

    return g_CurrentGamepadState[
        gamepad
    ].buttons[b] == GLFW_PRESS;

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    (void)button;
    (void)gamepad;

    return false;

#endif
}


bool IsGamepadPressed(
    GamepadButton button,
    int gamepad
)
{
#if defined(__MINGW64__)

    if (gamepad < 0 ||
        gamepad > GLFW_JOYSTICK_LAST)
        return false;

    if (!g_GamepadValid[gamepad])
        return false;

    int b = static_cast<int>(button);

    if (b < 0 ||
        b > GLFW_GAMEPAD_BUTTON_LAST)
        return false;

    bool current =
        g_CurrentGamepadState[
            gamepad
        ].buttons[b] == GLFW_PRESS;

    bool previous =
        g_PreviousGamepadValid[gamepad] &&
        g_PreviousGamepadState[
            gamepad
        ].buttons[b] == GLFW_PRESS;

    return current && !previous;

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    (void)button;
    (void)gamepad;

    return false;

#endif
}


bool IsGamepadReleased(
    GamepadButton button,
    int gamepad
)
{
#if defined(__MINGW64__)

    if (gamepad < 0 ||
        gamepad > GLFW_JOYSTICK_LAST)
        return false;

    int b = static_cast<int>(button);

    if (b < 0 ||
        b > GLFW_GAMEPAD_BUTTON_LAST)
        return false;

    bool current =
        g_GamepadValid[gamepad] &&
        g_CurrentGamepadState[
            gamepad
        ].buttons[b] == GLFW_PRESS;

    bool previous =
        g_PreviousGamepadValid[gamepad] &&
        g_PreviousGamepadState[
            gamepad
        ].buttons[b] == GLFW_PRESS;

    return !current && previous;

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    (void)button;
    (void)gamepad;

    return false;

#endif
}


float GetGamepadAxis(
    GamepadAxis axis,
    int gamepad
)
{
#if defined(__MINGW64__)

    if (gamepad < 0 ||
        gamepad > GLFW_JOYSTICK_LAST)
        return 0.0f;

    if (!g_GamepadValid[gamepad])
        return 0.0f;

    int a = static_cast<int>(axis);

    if (a < 0 ||
        a > GLFW_GAMEPAD_AXIS_LAST)
        return 0.0f;

    return g_CurrentGamepadState[
        gamepad
    ].axes[a];

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    (void)axis;
    (void)gamepad;

    return 0.0f;

#endif
}


const char* GetGamepadName(int gamepad)
{
#if defined(__MINGW64__)

    if (gamepad < 0 ||
        gamepad > GLFW_JOYSTICK_LAST)
        return nullptr;

    if (!glfwJoystickPresent(gamepad) ||
        !glfwJoystickIsGamepad(gamepad))
        return nullptr;

    return glfwGetGamepadName(gamepad);

#elif defined(__MINGW32__) && !defined(__MINGW64__)

    (void)gamepad;

    return nullptr;

#endif
}

} // namespace Input
} // namespace TWE