// #pragma once

// #include <GLFW/glfw3.h>

// namespace TWE
// {

//     namespace Input
//     {
//         // ============================================================
//         // Keyboard
//         // ============================================================

//         enum class Key
//         {
//             Unknown = GLFW_KEY_UNKNOWN,

//             Space       = GLFW_KEY_SPACE,
//             Apostrophe  = GLFW_KEY_APOSTROPHE,
//             Comma       = GLFW_KEY_COMMA,
//             Minus       = GLFW_KEY_MINUS,
//             Period      = GLFW_KEY_PERIOD,
//             Slash       = GLFW_KEY_SLASH,

//             Num0        = GLFW_KEY_0,
//             Num1        = GLFW_KEY_1,
//             Num2        = GLFW_KEY_2,
//             Num3        = GLFW_KEY_3,
//             Num4        = GLFW_KEY_4,
//             Num5        = GLFW_KEY_5,
//             Num6        = GLFW_KEY_6,
//             Num7        = GLFW_KEY_7,
//             Num8        = GLFW_KEY_8,
//             Num9        = GLFW_KEY_9,

//             Semicolon   = GLFW_KEY_SEMICOLON,
//             Equal       = GLFW_KEY_EQUAL,

//             A = GLFW_KEY_A,
//             B = GLFW_KEY_B,
//             C = GLFW_KEY_C,
//             D = GLFW_KEY_D,
//             E = GLFW_KEY_E,
//             F = GLFW_KEY_F,
//             G = GLFW_KEY_G,
//             H = GLFW_KEY_H,
//             I = GLFW_KEY_I,
//             J = GLFW_KEY_J,
//             K = GLFW_KEY_K,
//             L = GLFW_KEY_L,
//             M = GLFW_KEY_M,
//             N = GLFW_KEY_N,
//             O = GLFW_KEY_O,
//             P = GLFW_KEY_P,
//             Q = GLFW_KEY_Q,
//             R = GLFW_KEY_R,
//             S = GLFW_KEY_S,
//             T = GLFW_KEY_T,
//             U = GLFW_KEY_U,
//             V = GLFW_KEY_V,
//             W = GLFW_KEY_W,
//             X = GLFW_KEY_X,
//             Y = GLFW_KEY_Y,
//             Z = GLFW_KEY_Z,

//             LeftBracket  = GLFW_KEY_LEFT_BRACKET,
//             Backslash    = GLFW_KEY_BACKSLASH,
//             RightBracket = GLFW_KEY_RIGHT_BRACKET,
//             GraveAccent  = GLFW_KEY_GRAVE_ACCENT,

//             World1 = GLFW_KEY_WORLD_1,
//             World2 = GLFW_KEY_WORLD_2,

//             Escape    = GLFW_KEY_ESCAPE,
//             Enter     = GLFW_KEY_ENTER,
//             Tab       = GLFW_KEY_TAB,
//             Backspace = GLFW_KEY_BACKSPACE,
//             Insert    = GLFW_KEY_INSERT,
//             Delete    = GLFW_KEY_DELETE,

//             Right = GLFW_KEY_RIGHT,
//             Left  = GLFW_KEY_LEFT,
//             Down  = GLFW_KEY_DOWN,
//             Up    = GLFW_KEY_UP,

//             PageUp   = GLFW_KEY_PAGE_UP,
//             PageDown = GLFW_KEY_PAGE_DOWN,
//             Home     = GLFW_KEY_HOME,
//             End      = GLFW_KEY_END,

//             CapsLock   = GLFW_KEY_CAPS_LOCK,
//             ScrollLock = GLFW_KEY_SCROLL_LOCK,
//             NumLock    = GLFW_KEY_NUM_LOCK,
//             PrintScreen = GLFW_KEY_PRINT_SCREEN,
//             Pause      = GLFW_KEY_PAUSE,

//             F1  = GLFW_KEY_F1,
//             F2  = GLFW_KEY_F2,
//             F3  = GLFW_KEY_F3,
//             F4  = GLFW_KEY_F4,
//             F5  = GLFW_KEY_F5,
//             F6  = GLFW_KEY_F6,
//             F7  = GLFW_KEY_F7,
//             F8  = GLFW_KEY_F8,
//             F9  = GLFW_KEY_F9,
//             F10 = GLFW_KEY_F10,
//             F11 = GLFW_KEY_F11,
//             F12 = GLFW_KEY_F12,
//             F13 = GLFW_KEY_F13,
//             F14 = GLFW_KEY_F14,
//             F15 = GLFW_KEY_F15,
//             F16 = GLFW_KEY_F16,
//             F17 = GLFW_KEY_F17,
//             F18 = GLFW_KEY_F18,
//             F19 = GLFW_KEY_F19,
//             F20 = GLFW_KEY_F20,
//             F21 = GLFW_KEY_F21,
//             F22 = GLFW_KEY_F22,
//             F23 = GLFW_KEY_F23,
//             F24 = GLFW_KEY_F24,
//             F25 = GLFW_KEY_F25,

//             KP0 = GLFW_KEY_KP_0,
//             KP1 = GLFW_KEY_KP_1,
//             KP2 = GLFW_KEY_KP_2,
//             KP3 = GLFW_KEY_KP_3,
//             KP4 = GLFW_KEY_KP_4,
//             KP5 = GLFW_KEY_KP_5,
//             KP6 = GLFW_KEY_KP_6,
//             KP7 = GLFW_KEY_KP_7,
//             KP8 = GLFW_KEY_KP_8,
//             KP9 = GLFW_KEY_KP_9,

//             KPDecimal  = GLFW_KEY_KP_DECIMAL,
//             KPDivide   = GLFW_KEY_KP_DIVIDE,
//             KPMultiply = GLFW_KEY_KP_MULTIPLY,
//             KPSubtract = GLFW_KEY_KP_SUBTRACT,
//             KPAdd      = GLFW_KEY_KP_ADD,
//             KPEnter    = GLFW_KEY_KP_ENTER,
//             KPEqual    = GLFW_KEY_KP_EQUAL,

//             LeftShift    = GLFW_KEY_LEFT_SHIFT,
//             LeftControl  = GLFW_KEY_LEFT_CONTROL,
//             LeftAlt      = GLFW_KEY_LEFT_ALT,
//             LeftSuper    = GLFW_KEY_LEFT_SUPER,

//             RightShift   = GLFW_KEY_RIGHT_SHIFT,
//             RightControl = GLFW_KEY_RIGHT_CONTROL,
//             RightAlt     = GLFW_KEY_RIGHT_ALT,
//             RightSuper   = GLFW_KEY_RIGHT_SUPER,

//             Menu = GLFW_KEY_MENU
//         };


//         // ============================================================
//         // Mouse
//         // ============================================================

//         enum class MouseButton
//         {
//             Button1 = GLFW_MOUSE_BUTTON_1,
//             Button2 = GLFW_MOUSE_BUTTON_2,
//             Button3 = GLFW_MOUSE_BUTTON_3,
//             Button4 = GLFW_MOUSE_BUTTON_4,
//             Button5 = GLFW_MOUSE_BUTTON_5,
//             Button6 = GLFW_MOUSE_BUTTON_6,
//             Button7 = GLFW_MOUSE_BUTTON_7,
//             Button8 = GLFW_MOUSE_BUTTON_8,

//             Left   = GLFW_MOUSE_BUTTON_LEFT,
//             Right  = GLFW_MOUSE_BUTTON_RIGHT,
//             Middle = GLFW_MOUSE_BUTTON_MIDDLE
//         };


//         // ============================================================
//         // Gamepad Buttons
//         // ============================================================

//         enum class GamepadButton
//         {
//             A = GLFW_GAMEPAD_BUTTON_A,
//             B = GLFW_GAMEPAD_BUTTON_B,
//             X = GLFW_GAMEPAD_BUTTON_X,
//             Y = GLFW_GAMEPAD_BUTTON_Y,

//             LeftBumper  = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
//             RightBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,

//             Back  = GLFW_GAMEPAD_BUTTON_BACK,
//             Start = GLFW_GAMEPAD_BUTTON_START,

//             Guide = GLFW_GAMEPAD_BUTTON_GUIDE,

//             LeftThumb  = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
//             RightThumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,

//             DPadUp    = GLFW_GAMEPAD_BUTTON_DPAD_UP,
//             DPadRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
//             DPadDown  = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
//             DPadLeft  = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,

//             Cross    = GLFW_GAMEPAD_BUTTON_A,
//             Circle   = GLFW_GAMEPAD_BUTTON_B,
//             Square   = GLFW_GAMEPAD_BUTTON_X,
//             Triangle = GLFW_GAMEPAD_BUTTON_Y
//         };


//         // ============================================================
//         // Gamepad Axes
//         // ============================================================

//         enum class GamepadAxis
//         {
//             LeftX = GLFW_GAMEPAD_AXIS_LEFT_X,
//             LeftY = GLFW_GAMEPAD_AXIS_LEFT_Y,

//             RightX = GLFW_GAMEPAD_AXIS_RIGHT_X,
//             RightY = GLFW_GAMEPAD_AXIS_RIGHT_Y,

//             LeftTrigger  = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
//             RightTrigger = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER
//         };


//         // ============================================================
//         // Initialization
//         // ============================================================

//         void Init(GLFWwindow* window);

//         void Update();


//         // ============================================================
//         // Keyboard
//         // ============================================================

//         bool IsKeyDown(Key key);

//         bool IsKeyPressed(Key key);

//         bool IsKeyReleased(Key key);


//         // ============================================================
//         // Mouse
//         // ============================================================

//         bool IsMouseButtonDown(MouseButton button);

//         bool IsMouseButtonPressed(MouseButton button);

//         bool IsMouseButtonReleased(MouseButton button);

//         double GetMouseX();
//         double GetMouseY();

//         void GetMousePosition(double& x, double& y);


//         // ============================================================
//         // Gamepad
//         // ============================================================

//         bool IsGamepadConnected(int gamepad = GLFW_JOYSTICK_1);

//         bool IsGamepadDown(
//             GamepadButton button,
//             int gamepad = GLFW_JOYSTICK_1
//         );

//         bool IsGamepadPressed(
//             GamepadButton button,
//             int gamepad = GLFW_JOYSTICK_1
//         );

//         bool IsGamepadReleased(
//             GamepadButton button,
//             int gamepad = GLFW_JOYSTICK_1
//         );

//         float GetGamepadAxis(
//             GamepadAxis axis,
//             int gamepad = GLFW_JOYSTICK_1
//         );

//         const char* GetGamepadName(
//             int gamepad = GLFW_JOYSTICK_1
//         );
//     }
// }

#pragma once

// ============================================================
// GLFW 3
// ============================================================

#if defined(__MINGW64__)

    #define GLFW_INCLUDE_NONE
    #include <GLFW/glfw3.h>

// ============================================================
// GLFW 2
// ============================================================

#elif defined(__MINGW32__)

    #include <GLFW/glfw2.h>

#else

    #error "TimWorks Engine: Unsupported MinGW target."

#endif


namespace TWE
{

namespace Input
{

// ============================================================
// Keyboard
// ============================================================

enum class Key
{
#if defined(__MINGW64__)

    Unknown = GLFW_KEY_UNKNOWN,

    Space       = GLFW_KEY_SPACE,
    Apostrophe  = GLFW_KEY_APOSTROPHE,
    Comma       = GLFW_KEY_COMMA,
    Minus       = GLFW_KEY_MINUS,
    Period      = GLFW_KEY_PERIOD,
    Slash       = GLFW_KEY_SLASH,

    Num0 = GLFW_KEY_0,
    Num1 = GLFW_KEY_1,
    Num2 = GLFW_KEY_2,
    Num3 = GLFW_KEY_3,
    Num4 = GLFW_KEY_4,
    Num5 = GLFW_KEY_5,
    Num6 = GLFW_KEY_6,
    Num7 = GLFW_KEY_7,
    Num8 = GLFW_KEY_8,
    Num9 = GLFW_KEY_9,

    Semicolon = GLFW_KEY_SEMICOLON,
    Equal     = GLFW_KEY_EQUAL,

    A = GLFW_KEY_A,
    B = GLFW_KEY_B,
    C = GLFW_KEY_C,
    D = GLFW_KEY_D,
    E = GLFW_KEY_E,
    F = GLFW_KEY_F,
    G = GLFW_KEY_G,
    H = GLFW_KEY_H,
    I = GLFW_KEY_I,
    J = GLFW_KEY_J,
    K = GLFW_KEY_K,
    L = GLFW_KEY_L,
    M = GLFW_KEY_M,
    N = GLFW_KEY_N,
    O = GLFW_KEY_O,
    P = GLFW_KEY_P,
    Q = GLFW_KEY_Q,
    R = GLFW_KEY_R,
    S = GLFW_KEY_S,
    T = GLFW_KEY_T,
    U = GLFW_KEY_U,
    V = GLFW_KEY_V,
    W = GLFW_KEY_W,
    X = GLFW_KEY_X,
    Y = GLFW_KEY_Y,
    Z = GLFW_KEY_Z,

    LeftBracket  = GLFW_KEY_LEFT_BRACKET,
    Backslash    = GLFW_KEY_BACKSLASH,
    RightBracket = GLFW_KEY_RIGHT_BRACKET,
    GraveAccent  = GLFW_KEY_GRAVE_ACCENT,

    World1 = GLFW_KEY_WORLD_1,
    World2 = GLFW_KEY_WORLD_2,

    Escape    = GLFW_KEY_ESCAPE,
    Enter     = GLFW_KEY_ENTER,
    Tab       = GLFW_KEY_TAB,
    Backspace = GLFW_KEY_BACKSPACE,
    Insert    = GLFW_KEY_INSERT,
    Delete    = GLFW_KEY_DELETE,

    Right = GLFW_KEY_RIGHT,
    Left  = GLFW_KEY_LEFT,
    Down  = GLFW_KEY_DOWN,
    Up    = GLFW_KEY_UP,

    PageUp   = GLFW_KEY_PAGE_UP,
    PageDown = GLFW_KEY_PAGE_DOWN,
    Home     = GLFW_KEY_HOME,
    End      = GLFW_KEY_END,

    CapsLock    = GLFW_KEY_CAPS_LOCK,
    ScrollLock  = GLFW_KEY_SCROLL_LOCK,
    NumLock     = GLFW_KEY_NUM_LOCK,
    PrintScreen = GLFW_KEY_PRINT_SCREEN,
    Pause       = GLFW_KEY_PAUSE,

    F1  = GLFW_KEY_F1,
    F2  = GLFW_KEY_F2,
    F3  = GLFW_KEY_F3,
    F4  = GLFW_KEY_F4,
    F5  = GLFW_KEY_F5,
    F6  = GLFW_KEY_F6,
    F7  = GLFW_KEY_F7,
    F8  = GLFW_KEY_F8,
    F9  = GLFW_KEY_F9,
    F10 = GLFW_KEY_F10,
    F11 = GLFW_KEY_F11,
    F12 = GLFW_KEY_F12,
    F13 = GLFW_KEY_F13,
    F14 = GLFW_KEY_F14,
    F15 = GLFW_KEY_F15,
    F16 = GLFW_KEY_F16,
    F17 = GLFW_KEY_F17,
    F18 = GLFW_KEY_F18,
    F19 = GLFW_KEY_F19,
    F20 = GLFW_KEY_F20,
    F21 = GLFW_KEY_F21,
    F22 = GLFW_KEY_F22,
    F23 = GLFW_KEY_F23,
    F24 = GLFW_KEY_F24,
    F25 = GLFW_KEY_F25,

    KP0 = GLFW_KEY_KP_0,
    KP1 = GLFW_KEY_KP_1,
    KP2 = GLFW_KEY_KP_2,
    KP3 = GLFW_KEY_KP_3,
    KP4 = GLFW_KEY_KP_4,
    KP5 = GLFW_KEY_KP_5,
    KP6 = GLFW_KEY_KP_6,
    KP7 = GLFW_KEY_KP_7,
    KP8 = GLFW_KEY_KP_8,
    KP9 = GLFW_KEY_KP_9,

    KPDecimal  = GLFW_KEY_KP_DECIMAL,
    KPDivide   = GLFW_KEY_KP_DIVIDE,
    KPMultiply = GLFW_KEY_KP_MULTIPLY,
    KPSubtract = GLFW_KEY_KP_SUBTRACT,
    KPAdd      = GLFW_KEY_KP_ADD,
    KPEnter    = GLFW_KEY_KP_ENTER,
    KPEqual    = GLFW_KEY_KP_EQUAL,

    LeftShift   = GLFW_KEY_LEFT_SHIFT,
    LeftControl = GLFW_KEY_LEFT_CONTROL,
    LeftAlt     = GLFW_KEY_LEFT_ALT,
    LeftSuper   = GLFW_KEY_LEFT_SUPER,

    RightShift   = GLFW_KEY_RIGHT_SHIFT,
    RightControl = GLFW_KEY_RIGHT_CONTROL,
    RightAlt     = GLFW_KEY_RIGHT_ALT,
    RightSuper   = GLFW_KEY_RIGHT_SUPER,

    Menu = GLFW_KEY_MENU

#elif defined(__MINGW32__)

    /*
        GLFW2 uses different key names/values.

        These names assume your renamed GLFW2 header follows
        the original GLFW2 API.
    */

    Unknown = GLFW2_KEY_UNKNOWN,

    Space      = GLFW2_KEY_SPACE,
    Apostrophe = GLFW2_KEY_SPECIAL + 1,
    Comma      = GLFW2_KEY_COMMA,
    Minus      = GLFW2_KEY_MINUS,
    Period     = GLFW2_KEY_PERIOD,
    Slash      = GLFW2_KEY_SLASH,

    Num0 = '0',
    Num1 = '1',
    Num2 = '2',
    Num3 = '3',
    Num4 = '4',
    Num5 = '5',
    Num6 = '6',
    Num7 = '7',
    Num8 = '8',
    Num9 = '9',

    Semicolon = GLFW2_KEY_SEMICOLON,
    Equal     = GLFW2_KEY_EQUAL,

    A = 'A',
    B = 'B',
    C = 'C',
    D = 'D',
    E = 'E',
    F = 'F',
    G = 'G',
    H = 'H',
    I = 'I',
    J = 'J',
    K = 'K',
    L = 'L',
    M = 'M',
    N = 'N',
    O = 'O',
    P = 'P',
    Q = 'Q',
    R = 'R',
    S = 'S',
    T = 'T',
    U = 'U',
    V = 'V',
    W = 'W',
    X = 'X',
    Y = 'Y',
    Z = 'Z',

    Escape    = GLFW2_KEY_ESC,
    Enter     = GLFW2_KEY_ENTER,
    Tab       = GLFW2_KEY_TAB,
    Backspace = GLFW2_KEY_BACKSPACE,

    Insert = GLFW2_KEY_INSERT,
    Delete = GLFW2_KEY_DEL,

    Right = GLFW2_KEY_RIGHT,
    Left  = GLFW2_KEY_LEFT,
    Down  = GLFW2_KEY_DOWN,
    Up    = GLFW2_KEY_UP,

    PageUp   = GLFW2_KEY_PAGEUP,
    PageDown = GLFW2_KEY_PAGEDOWN,
    Home     = GLFW2_KEY_HOME,
    End      = GLFW2_KEY_END,

    CapsLock   = GLFW2_KEY_CAPSLOCK,
    ScrollLock = GLFW2_KEY_SCROLLOCK,
    NumLock    = GLFW2_KEY_NUMLOCK,
    Pause      = GLFW2_KEY_PAUSE,

    F1  = GLFW2_KEY_F1,
    F2  = GLFW2_KEY_F2,
    F3  = GLFW2_KEY_F3,
    F4  = GLFW2_KEY_F4,
    F5  = GLFW2_KEY_F5,
    F6  = GLFW2_KEY_F6,
    F7  = GLFW2_KEY_F7,
    F8  = GLFW2_KEY_F8,
    F9  = GLFW2_KEY_F9,
    F10 = GLFW2_KEY_F10,
    F11 = GLFW2_KEY_F11,
    F12 = GLFW2_KEY_F12,

    LeftShift   = GLFW2_KEY_LSHIFT,
    LeftControl = GLFW2_KEY_LCTRL,
    LeftAlt     = GLFW2_KEY_LALT,

    RightShift   = GLFW2_KEY_RSHIFT,
    RightControl = GLFW2_KEY_RCTRL,
    RightAlt     = GLFW2_KEY_RALT

#endif
};


// ============================================================
// Mouse
// ============================================================

enum class MouseButton
{
#if defined(__MINGW64__)

    Button1 = GLFW_MOUSE_BUTTON_1,
    Button2 = GLFW_MOUSE_BUTTON_2,
    Button3 = GLFW_MOUSE_BUTTON_3,
    Button4 = GLFW_MOUSE_BUTTON_4,
    Button5 = GLFW_MOUSE_BUTTON_5,
    Button6 = GLFW_MOUSE_BUTTON_6,
    Button7 = GLFW_MOUSE_BUTTON_7,
    Button8 = GLFW_MOUSE_BUTTON_8,

    Left   = GLFW_MOUSE_BUTTON_LEFT,
    Right  = GLFW_MOUSE_BUTTON_RIGHT,
    Middle = GLFW_MOUSE_BUTTON_MIDDLE

#elif defined(__MINGW32__)

    Button1 = GLFW2_MOUSE_BUTTON_1,
    Button2 = GLFW2_MOUSE_BUTTON_2,
    Button3 = GLFW2_MOUSE_BUTTON_3,
    Button4 = GLFW2_MOUSE_BUTTON_4,
    Button5 = GLFW2_MOUSE_BUTTON_5,
    Button6 = GLFW2_MOUSE_BUTTON_6,
    Button7 = GLFW2_MOUSE_BUTTON_7,
    Button8 = GLFW2_MOUSE_BUTTON_8,

    Left   = GLFW2_MOUSE_BUTTON_1,
    Right  = GLFW2_MOUSE_BUTTON_2,
    Middle = GLFW2_MOUSE_BUTTON_3

#endif
};


// ============================================================
// Gamepad
// ============================================================

enum class GamepadButton
{
#if defined(__MINGW64__)

    A = GLFW_GAMEPAD_BUTTON_A,
    B = GLFW_GAMEPAD_BUTTON_B,
    X = GLFW_GAMEPAD_BUTTON_X,
    Y = GLFW_GAMEPAD_BUTTON_Y,

    LeftBumper  = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
    RightBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,

    Back  = GLFW_GAMEPAD_BUTTON_BACK,
    Start = GLFW_GAMEPAD_BUTTON_START,

    Guide = GLFW_GAMEPAD_BUTTON_GUIDE,

    LeftThumb  = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
    RightThumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,

    DPadUp    = GLFW_GAMEPAD_BUTTON_DPAD_UP,
    DPadRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
    DPadDown  = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
    DPadLeft  = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,

    Cross    = GLFW_GAMEPAD_BUTTON_A,
    Circle   = GLFW_GAMEPAD_BUTTON_B,
    Square   = GLFW_GAMEPAD_BUTTON_X,
    Triangle = GLFW_GAMEPAD_BUTTON_Y

#elif defined(__MINGW32__)

    /*
        GLFW2 has no standardized gamepad API.
    */

    A = 0,
    B = 1,
    X = 2,
    Y = 3,

    LeftBumper  = 4,
    RightBumper = 5,

    Back  = 6,
    Start = 7,

    Guide = 8,

    LeftThumb  = 9,
    RightThumb = 10,

    DPadUp    = 11,
    DPadRight = 12,
    DPadDown  = 13,
    DPadLeft  = 14,

    Cross    = A,
    Circle   = B,
    Square   = X,
    Triangle = Y

#endif
};


// ============================================================
// Gamepad Axes
// ============================================================

enum class GamepadAxis
{
#if defined(__MINGW64__)

    LeftX = GLFW_GAMEPAD_AXIS_LEFT_X,
    LeftY = GLFW_GAMEPAD_AXIS_LEFT_Y,

    RightX = GLFW_GAMEPAD_AXIS_RIGHT_X,
    RightY = GLFW_GAMEPAD_AXIS_RIGHT_Y,

    LeftTrigger  = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
    RightTrigger = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER

#elif defined(__MINGW32__)

    LeftX = 0,
    LeftY = 1,

    RightX = 2,
    RightY = 3,

    LeftTrigger  = 4,
    RightTrigger = 5

#endif
};


// ============================================================
// Initialization
// ============================================================

#if defined(__MINGW64__)

void Init(GLFWwindow* window);

#elif defined(__MINGW32__)

void Init(void* window);

#endif

void Update();


// ============================================================
// Keyboard
// ============================================================

bool IsKeyDown(Key key);
bool IsKeyPressed(Key key);
bool IsKeyReleased(Key key);


// ============================================================
// Mouse
// ============================================================

bool IsMouseButtonDown(MouseButton button);
bool IsMouseButtonPressed(MouseButton button);
bool IsMouseButtonReleased(MouseButton button);

double GetMouseX();
double GetMouseY();

void GetMousePosition(double& x, double& y);


// ============================================================
// Gamepad
// ============================================================

bool IsGamepadConnected(
    int gamepad = 0
);

bool IsGamepadDown(
    GamepadButton button,
    int gamepad = 0
);

bool IsGamepadPressed(
    GamepadButton button,
    int gamepad = 0
);

bool IsGamepadReleased(
    GamepadButton button,
    int gamepad = 0
);

float GetGamepadAxis(
    GamepadAxis axis,
    int gamepad = 0
);

const char* GetGamepadName(
    int gamepad = 0
);

} // namespace Input

} // namespace TWE