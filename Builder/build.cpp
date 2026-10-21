/* #include <cstdlib>
// #include <iostream>
// #include <string>
// #include <thread>
// #include <mutex>
// #include <vector>
// #include <atomic>
// #include <algorithm>

// #define WIN32_LEAN_AND_MEAN
// #define NOGDI
// #define NOUSER

// #include <windows.h>

// #include "raylib/raylib.h"
// #include "ImGUI/imgui.h"
// #include "rlImGUI/rlImGui.h"


// // ================================================================
// // Application                                                                                                 
// // ================================================================

// std::string currentVersion = "App version: 0.0.7";


// // ================================================================
// // Console
// // ================================================================

// std::string consoleOutput;
// std::mutex consoleMutex;

// constexpr size_t MAX_CONSOLE_SIZE = 10 * 1024 * 1024; // 10 MB


// // ================================================================
// // Command threads
// // ================================================================

// std::vector<std::thread> commandThreads;
// std::mutex commandMutex;

// std::atomic<bool> commandRunning = false;


// // ================================================================
// // Help
// // ================================================================

// void PrintHelp(void)
// {
//     std::cout << "Available commands:\n";
//     std::cout << "  help\n";
//     std::cout << "  -help\n";
//     std::cout << "  --help\n";
//     std::cout << "  --version\n";
//     std::cout << "  --buildEngine\n";
//     std::cout << "  --buildLevelEditor\n";
//     std::cout << "  --runEngine\n";
//     std::cout << "  --runLevelEditor\n";
// }


// // ================================================================
// // Add output safely
// // ================================================================

// void AppendConsoleOutput(const char* data, size_t size)
// {
//     std::lock_guard<std::mutex> lock(consoleMutex);

//     consoleOutput.append(data, size);

//     // Prevent unlimited memory growth.
//     if (consoleOutput.size() > MAX_CONSOLE_SIZE)
//     {
//         size_t amountToRemove =
//             consoleOutput.size() - MAX_CONSOLE_SIZE;

//         consoleOutput.erase(
//             0,
//             amountToRemove
//         );
//     }
// }


// // ================================================================
// // Run command
// // ================================================================

// void RunCommand(const std::string& command)
// {
//     // ------------------------------------------------------------
//     // Prevent multiple commands from running simultaneously
//     // ------------------------------------------------------------

//     bool expected = false;

//     if (!commandRunning.compare_exchange_strong(expected, true))
//     {
//         AppendConsoleOutput(
//             "\n[Builder] A command is already running.\n",
//             43
//         );

//         return;
//     }


//     // ------------------------------------------------------------
//     // Create pipe
//     // ------------------------------------------------------------

//     SECURITY_ATTRIBUTES sa{};

//     sa.nLength = sizeof(SECURITY_ATTRIBUTES);
//     sa.bInheritHandle = TRUE;
//     sa.lpSecurityDescriptor = nullptr;

//     HANDLE hRead = nullptr;
//     HANDLE hWrite = nullptr;

//     if (!CreatePipe(
//         &hRead,
//         &hWrite,
//         &sa,
//         0))
//     {
//         AppendConsoleOutput(
//             "\n[Builder] Failed to create output pipe.\n",
//             42
//         );

//         commandRunning = false;
//         return;
//     }


//     // The parent must not inherit the read handle.

//     SetHandleInformation(
//         hRead,
//         HANDLE_FLAG_INHERIT,
//         0
//     );


//     // ------------------------------------------------------------
//     // Create process
//     // ------------------------------------------------------------

//     STARTUPINFOA si{};

//     PROCESS_INFORMATION pi{};

//     si.cb = sizeof(STARTUPINFOA);

//     si.dwFlags |= STARTF_USESTDHANDLES;

//     si.hStdOutput = hWrite;
//     si.hStdError = hWrite;


//     // std::string cmdLine =
//     //     "cmd.exe /C " + command;

//     const std::string MSYS2_BASH =
//         R"(E:\msys64\usr\bin\bash.exe)";

//     std::string cmdLine;

//     if (command.rfind("UCRT64:", 0) == 0)
//     {
//         std::string bashCommand =
//             command.substr(7);

//         cmdLine =
//             "\"" + MSYS2_BASH + "\" -lc \"" +
//             bashCommand +
//             "\"";
//     }
//     else
//     {
//         cmdLine =
//             "cmd.exe /C " + command;
//     }

//     BOOL processCreated = CreateProcessA(
//         nullptr,
//         cmdLine.data(),
//         nullptr,
//         nullptr,
//         TRUE,
//         CREATE_NO_WINDOW,
//         nullptr,
//         nullptr,
//         &si,
//         &pi
//     );


//     if (!processCreated)
//     {
//         AppendConsoleOutput(
//             "\n[Builder] Failed to start command.\n",
//             37
//         );

//         CloseHandle(hWrite);
//         CloseHandle(hRead);

//         commandRunning = false;

//         return;
//     }


//     // ------------------------------------------------------------
//     // Parent no longer needs the write handle
//     // ------------------------------------------------------------

//     CloseHandle(hWrite);
//     hWrite = nullptr;


//     // ------------------------------------------------------------
//     // Wait for process
//     // ------------------------------------------------------------

//     WaitForSingleObject(
//         pi.hProcess,
//         INFINITE
//     );

//     // ------------------------------------------------------------
//     // Read command output
//     // ------------------------------------------------------------

//     char buffer[4096];

//     DWORD bytesRead = 0;
    

//     while (ReadFile(
//         hRead,
//         buffer,
//         sizeof(buffer),
//         &bytesRead,
//         nullptr)
//         && bytesRead > 0)
//     {
//         AppendConsoleOutput(
//             buffer,
//             bytesRead
//         );
//     }


//     // ------------------------------------------------------------
//     // Cleanup process handles
//     // ------------------------------------------------------------

//     CloseHandle(pi.hProcess);
//     CloseHandle(pi.hThread);

//     CloseHandle(hRead);


//     // ------------------------------------------------------------
//     // Command finished
//     // ------------------------------------------------------------

//     commandRunning = false;
// }


// // ================================================================
// // Start command
// // ================================================================

// void StartCommand(const std::string& command)
// {
//     std::lock_guard<std::mutex> lock(commandMutex);

//     commandThreads.emplace_back(
//         RunCommand,
//         command
//     );
// }


// // ================================================================
// // Join all command threads
// // ================================================================

// void JoinCommandThreads()
// {
//     std::lock_guard<std::mutex> lock(commandMutex);

//     for (std::thread& thread : commandThreads)
//     {
//         if (thread.joinable())
//         {
//             thread.join();
//         }
//     }

//     commandThreads.clear();
// }


// // ================================================================
// // Deinitialize
// // ================================================================

// void deInit()
// {
//     // Make sure background commands are finished
//     // before destroying the GUI.

//     JoinCommandThreads();

//     rlImGuiShutdown();

//     CloseWindow();
// }


// // ================================================================
// // Main
// // ================================================================

// int main(int argc, char* argv[])
// {
//     // ============================================================
//     // Command line mode
//     // ============================================================

//     if (argc >= 2)
//     {
//         std::string cmd = argv[1];


//         if (cmd == "help" ||
//             cmd == "-help" ||
//             cmd == "--help")
//         {
//             PrintHelp();

//             return 0;
//         }


//         else if (cmd == "--version")
//         {
//             std::cout
//                 << currentVersion
//                 << std::endl;

//             return 0;
//         }


//         else if (cmd == "--buildEngine")
//         {
//             std::cout
//                 << "Building the Engine\n";

//             return std::system(
//                 "buildEngine.bat"
//             );
//         }


//         else if (cmd == "--buildLevelEditor")
//         {
//             std::cout
//                 << "Building the Level Editor\n";

//             return std::system(
//                 "buildLevelEditor.bat"
//             );
//         }


//         else if (cmd == "--runEngine")
//         {
//             std::cout
//                 << "Running the Engine\n";

//             return std::system(
//                 "runEngine.bat"
//             );
//         }


//         else if (cmd == "--runLevelEditor")
//         {
//             std::cout
//                 << "Running the Level Editor\n";

//             return std::system(
//                 "runLevelEditor.bat"
//             );
//         }


//         else
//         {
//             std::cout
//                 << "Unknown command.\n\n";

//             PrintHelp();

//             return 1;
//         }
//     }


//     // ============================================================
//     // Raylib
//     // ============================================================

//     SetTraceLogLevel(LOG_ERROR);

//     SetConfigFlags(
//         FLAG_WINDOW_RESIZABLE
//     );

//     InitWindow(
//         1280,
//         720,
//         "Builder"
//     );

//     rlImGuiSetup(true);


//     // ============================================================
//     // GUI state
//     // ============================================================

//     bool showAbout = false;

//     bool quitRequested = false;


//     // ============================================================
//     // Main loop
//     // ============================================================

//     while (!WindowShouldClose())
//     {
//         BeginDrawing();

//         ClearBackground(DARKGRAY);

//         rlImGuiBegin();


//         // ========================================================
//         // Main menu
//         // ========================================================

//         if (ImGui::BeginMainMenuBar())
//         {
//             // ----------------------------------------------------
//             // About
//             // ----------------------------------------------------

//             if (ImGui::BeginMenu("About"))
//             {
//                 if (ImGui::MenuItem("About"))
//                 {
//                     showAbout = true;
//                 }


//                 if (ImGui::MenuItem("Clear Console"))
//                 {
//                     std::lock_guard<std::mutex> lock(
//                         consoleMutex
//                     );

//                     consoleOutput.clear();
//                 }


//                 ImGui::EndMenu();
//             }


//             // ----------------------------------------------------
//             // Build
//             // ----------------------------------------------------

//             if (ImGui::BeginMenu("Build"))
//             {
//                 if (ImGui::MenuItem(
//                     "Build Engine",
//                     nullptr,
//                     false,
//                     !commandRunning.load()))
//                 {
//                     StartCommand(
//                         "buildEngine.bat"
//                     );
//                 }


//                 if (ImGui::MenuItem(
//                     "Build Level Editor",
//                     nullptr,
//                     false,
//                     !commandRunning.load()))
//                 {
//                     StartCommand(
//                         "buildLevelEditor.bat"
//                     );
//                 }


//                 ImGui::EndMenu();
//             }


//             // ----------------------------------------------------
//             // Run
//             // ----------------------------------------------------

//             if (ImGui::BeginMenu("Run"))
//             {
//                 if (ImGui::MenuItem(
//                     "Run Engine",
//                     nullptr,
//                     false,
//                     !commandRunning.load()))
//                 {
//                     StartCommand(
//                         "runEngine.bat"
//                     );
//                 }


//                 if (ImGui::MenuItem(
//                     "Run Level Editor",
//                     nullptr,
//                     false,
//                     !commandRunning.load()))
//                 {
//                     StartCommand(
//                         "runLevelEditor.bat"
//                     );
//                 }


//                 ImGui::EndMenu();
//             }


//             // ----------------------------------------------------
//             // Quit
//             // ----------------------------------------------------

//             if (ImGui::BeginMenu("Quit"))
//             {
//                 if (ImGui::MenuItem("Quit"))
//                 {
//                     quitRequested = true;
//                 }

//                 ImGui::EndMenu();
//             }


//             ImGui::EndMainMenuBar();
//         }


//         // ========================================================
//         // About window
//         // ========================================================

//         if (showAbout)
//         {
//             ImGui::Begin(
//                 "About",
//                 &showAbout
//             );


//             ImGui::Text(
//                 "%s",
//                 currentVersion.c_str()
//             );


//             ImGui::Text(
//                 "This is Something I made to make compiling "
//                 "my projects easier!"
//             );


//             ImGui::End();
//         }


//         // ========================================================
//         // Console
//         // ========================================================

//         ImGui::SetNextWindowPos(
//             ImVec2(0, 25),
//             ImGuiCond_Always
//         );


//         ImGui::SetNextWindowSize(
//             ImVec2(1280, 695),
//             ImGuiCond_Always
//         );


//         ImGui::Begin(
//             "Console",
//             nullptr,
//             ImGuiWindowFlags_NoMove
//         );


//         {
//             std::lock_guard<std::mutex> lock(
//                 consoleMutex
//             );


//             ImGui::BeginChild(
//                 "ConsoleOutput",
//                 ImVec2(0, 0),
//                 true,
//                 ImGuiWindowFlags_HorizontalScrollbar
//             );


//             ImGui::InputTextMultiline(
//                 "##ConsoleText",
//                 consoleOutput.data(),
//                 consoleOutput.size() + 1,
//                 ImVec2(-1, -1),
//                 ImGuiInputTextFlags_ReadOnly
//             );


//             ImGui::EndChild();
//         }


//         ImGui::End();


//         // ========================================================
//         // End frame
//         // ========================================================

//         rlImGuiEnd();

//         EndDrawing();


//         // ========================================================
//         // Quit after the frame is completely finished
//         // ========================================================

//         if (quitRequested)
//         {
//             break;
//         }
//     }


//     // ============================================================
//     // Shutdown
//     // ============================================================

//     deInit();

//     return 0;
// }*/

#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <algorithm>
#include <chrono>

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER

#include <windows.h>

#include "raylib/raylib.h"
#include "ImGUI/imgui.h"
#include "rlImGUI/rlImGui.h"


// ================================================================
// Application
// ================================================================

std::string currentVersion = "App version: 0.0.8";


// ================================================================
// Console
// ================================================================

std::string consoleOutput;
std::mutex consoleMutex;

constexpr size_t MAX_CONSOLE_SIZE = 10 * 1024 * 1024;


// ================================================================
// Command threads
// ================================================================

std::vector<std::thread> commandThreads;
std::mutex commandMutex;

std::atomic<bool> commandRunning = false;


// ================================================================
// Help
// ================================================================

void PrintHelp()
{
    std::cout << "Available commands:\n";
    std::cout << "  help\n";
    std::cout << "  -help\n";
    std::cout << "  --help\n";
    std::cout << "  --version\n";
    std::cout << "  --buildEngine\n";
    std::cout << "  --buildGame\n";
    std::cout << "  --buildLevelEditor\n";
    std::cout << "  --runEngine\n";
    std::cout << "  --runLevelEditor\n";
}


// ================================================================
// Append console output
// ================================================================

void AppendConsoleOutput(const char* data, size_t size)
{
    if (data == nullptr || size == 0)
        return;

    std::lock_guard<std::mutex> lock(consoleMutex);

    consoleOutput.append(data, size);

    if (consoleOutput.size() > MAX_CONSOLE_SIZE)
    {
        const size_t amountToRemove =
            consoleOutput.size() - MAX_CONSOLE_SIZE;

        consoleOutput.erase(
            0,
            amountToRemove
        );
    }
}


// ================================================================
// Append string
// ================================================================

void AppendConsoleOutput(const std::string& text)
{
    AppendConsoleOutput(
        text.data(),
        text.size()
    );
}


// ================================================================
// Run command
// ================================================================

void RunCommand(const std::string& command)
{
    // ------------------------------------------------------------
    // Prevent multiple commands from running simultaneously
    // ------------------------------------------------------------

    bool expected = false;

    if (!commandRunning.compare_exchange_strong(
        expected,
        true))
    {
        AppendConsoleOutput(
            "\n[Builder] A command is already running.\n"
        );

        return;
    }


    // ------------------------------------------------------------
    // Create pipe
    // ------------------------------------------------------------

    SECURITY_ATTRIBUTES sa{};

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = nullptr;

    HANDLE hRead = nullptr;
    HANDLE hWrite = nullptr;

    if (!CreatePipe(
        &hRead,
        &hWrite,
        &sa,
        0))
    {
        AppendConsoleOutput(
            "\n[Builder] Failed to create output pipe.\n"
        );

        commandRunning = false;
        return;
    }


    // Parent must not inherit read handle.

    if (!SetHandleInformation(
        hRead,
        HANDLE_FLAG_INHERIT,
        0))
    {
        AppendConsoleOutput(
            "\n[Builder] Failed to configure output pipe.\n"
        );

        CloseHandle(hRead);
        CloseHandle(hWrite);

        commandRunning = false;
        return;
    }


    // ------------------------------------------------------------
    // Prepare process startup information
    // ------------------------------------------------------------

    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};

    si.cb = sizeof(STARTUPINFOA);

    si.dwFlags |= STARTF_USESTDHANDLES;

    si.hStdOutput = hWrite;
    si.hStdError  = hWrite;


    // ------------------------------------------------------------
    // Build command line
    // ------------------------------------------------------------

    const std::string MSYS2_BASH =
        R"(E:\msys64\usr\bin\bash.exe)";

    std::string cmdLine;

    if (command.rfind("UCRT64:", 0) == 0)
    {
        const std::string bashCommand =
            command.substr(7);

        cmdLine =
            "\"" +
            MSYS2_BASH +
            "\" -lc \"" +
            bashCommand +
            "\"";
    }
    else
    {
        cmdLine =
            "cmd.exe /C " +
            command;
    }


    // ------------------------------------------------------------
    // Start process
    // ------------------------------------------------------------

    BOOL processCreated = CreateProcessA(
        nullptr,
        cmdLine.data(),
        nullptr,
        nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &si,
        &pi
    );


    if (!processCreated)
    {
        const DWORD errorCode =
            GetLastError();

        AppendConsoleOutput(
            "\n[Builder] Failed to start command.\n"
        );

        AppendConsoleOutput(
            "[Builder] Windows error code: " +
            std::to_string(errorCode) +
            "\n"
        );

        CloseHandle(hRead);
        CloseHandle(hWrite);

        commandRunning = false;

        return;
    }


    // ------------------------------------------------------------
    // Parent no longer needs write handle
    // ------------------------------------------------------------

    CloseHandle(hWrite);
    hWrite = nullptr;


    // ============================================================
    // IMPORTANT:
    //
    // Read the pipe BEFORE waiting for the process to finish.
    //
    // This prevents a pipe-buffer deadlock when g++ produces a lot
    // of compiler/linker output.
    // ============================================================

    char buffer[4096];

    DWORD bytesRead = 0;

    while (true)
    {
        BOOL success = ReadFile(
            hRead,
            buffer,
            sizeof(buffer),
            &bytesRead,
            nullptr
        );

        if (success && bytesRead > 0)
        {
            AppendConsoleOutput(
                buffer,
                bytesRead
            );

            continue;
        }

        // --------------------------------------------------------
        // No more data currently available.
        //
        // Check whether the process has finished.
        // --------------------------------------------------------

        DWORD waitResult =
            WaitForSingleObject(
                pi.hProcess,
                10
            );

        if (waitResult == WAIT_OBJECT_0)
        {
            // Process has exited.
            //
            // There may still be data left in the pipe, so continue
            // reading until the pipe reaches EOF.

            while (ReadFile(
                hRead,
                buffer,
                sizeof(buffer),
                &bytesRead,
                nullptr)
                && bytesRead > 0)
            {
                AppendConsoleOutput(
                    buffer,
                    bytesRead
                );
            }

            break;
        }

        if (waitResult == WAIT_FAILED)
        {
            AppendConsoleOutput(
                "\n[Builder] WaitForSingleObject failed.\n"
            );

            break;
        }

        // Process is still running.
        //
        // Give the GUI thread time to continue running normally.
        std::this_thread::yield();
    }


    // ------------------------------------------------------------
    // Get exit code
    // ------------------------------------------------------------

    DWORD exitCode = 0;

    if (GetExitCodeProcess(
        pi.hProcess,
        &exitCode))
    {
        if (exitCode == 0)
        {
            AppendConsoleOutput(
                "\n[Builder] Command completed successfully.\n"
            );
        }
        else
        {
            AppendConsoleOutput(
                "\n[Builder] Command failed with exit code " +
                std::to_string(exitCode) +
                ".\n"
            );
        }
    }


    // ------------------------------------------------------------
    // Cleanup
    // ------------------------------------------------------------

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hRead);


    // ------------------------------------------------------------
    // Command finished
    // ------------------------------------------------------------

    commandRunning = false;
}


// ================================================================
// Start command
// ================================================================

void StartCommand(const std::string& command)
{
    std::lock_guard<std::mutex> lock(
        commandMutex
    );

    commandThreads.emplace_back(
        RunCommand,
        command
    );
}


// ================================================================
// Join command threads
// ================================================================

void JoinCommandThreads()
{
    std::lock_guard<std::mutex> lock(
        commandMutex
    );

    for (std::thread& thread : commandThreads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    commandThreads.clear();
}


// ================================================================
// Deinitialize
// ================================================================

void deInit()
{
    // ------------------------------------------------------------
    // Wait for background commands before destroying GUI.
    // ------------------------------------------------------------

    JoinCommandThreads();

    rlImGuiShutdown();

    CloseWindow();
}


// ================================================================
// Main
// ================================================================

int main(int argc, char* argv[])
{
    // ============================================================
    // Command line mode
    // ============================================================

    if (argc >= 2)
    {
        std::string cmd = argv[1];


        // --------------------------------------------------------
        // Help
        // --------------------------------------------------------

        if (cmd == "help" ||
            cmd == "-help" ||
            cmd == "--help")
        {
            PrintHelp();

            return 0;
        }


        // --------------------------------------------------------
        // Version
        // --------------------------------------------------------

        if (cmd == "--version")
        {
            std::cout
                << currentVersion
                << std::endl;

            return 0;
        }


        // --------------------------------------------------------
        // Build Engine
        // --------------------------------------------------------

        if (cmd == "--buildEngine")
        {
            std::cout
                << "Building the Engine\n";

            return std::system(
                "buildEngine.bat"
            );
        }

        if (cmd == "--buildGame")
        {
            std::cout
                << "Building the Game Executable\n";

            return std::system(
                "buildGame.bat"
            );
        }


        // --------------------------------------------------------
        // Build Level Editor
        // --------------------------------------------------------

        if (cmd == "--buildLevelEditor")
        {
            std::cout
                << "Building the Level Editor\n";

            return std::system(
                "buildLevelEditor.bat"
            );
        }


        // --------------------------------------------------------
        // Run Engine
        // --------------------------------------------------------

        if (cmd == "--runEngine")
        {
            std::cout
                << "Running the Engine\n";

            return std::system(
                "runEngine.bat"
            );
        }


        // --------------------------------------------------------
        // Run Level Editor
        // --------------------------------------------------------

        if (cmd == "--runLevelEditor")
        {
            std::cout
                << "Running the Level Editor\n";

            return std::system(
                "runLevelEditor.bat"
            );
        }


        // --------------------------------------------------------
        // Unknown command
        // --------------------------------------------------------

        std::cout
            << "Unknown command.\n\n";

        PrintHelp();

        return 1;
    }


    // ============================================================
    // Raylib
    // ============================================================

    SetTraceLogLevel(LOG_ERROR);

    SetConfigFlags(
        FLAG_WINDOW_RESIZABLE
    );

    InitWindow(
        1280,
        720,
        "TimWorks Builder"
    );

    rlImGuiSetup(true);


    // ============================================================
    // GUI state
    // ============================================================

    bool showAbout = false;
    bool quitRequested = false;

    bool consoleAutoScroll = true;


    // ============================================================
    // Main loop
    // ============================================================

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(DARKGRAY);

        rlImGuiBegin();


        // ========================================================
        // Main menu
        // ========================================================

        if (ImGui::BeginMainMenuBar())
        {
            // ----------------------------------------------------
            // About
            // ----------------------------------------------------

            if (ImGui::BeginMenu("About"))
            {
                if (ImGui::MenuItem("About"))
                {
                    showAbout = true;
                }


                if (ImGui::MenuItem("Clear Console"))
                {
                    std::lock_guard<std::mutex> lock(
                        consoleMutex
                    );

                    consoleOutput.clear();
                }


                ImGui::EndMenu();
            }


            // ----------------------------------------------------
            // Build
            // ----------------------------------------------------

            if (ImGui::BeginMenu("Build"))
            {
                const bool canRun =
                    !commandRunning.load();


                if (ImGui::MenuItem(
                    "Build Engine",
                    nullptr,
                    false,
                    canRun))
                {
                    StartCommand(
                        "buildEngine.bat"
                    );
                }

                if (ImGui::MenuItem(
                    "Build Game",
                    nullptr,
                    false,
                    canRun))
                {
                    StartCommand(
                        "buildGame.bat"
                    );
                }

                if (ImGui::MenuItem(
                    "Build Model Maker",
                    nullptr,
                    false,
                    canRun))
                {
                    StartCommand(
                        "buildModelMaker.bat"
                    );
                }


                if (ImGui::MenuItem(
                    "Build Level Editor",
                    nullptr,
                    false,
                    canRun))
                {
                    StartCommand(
                        "buildLevelEditor.bat"
                    );
                }


                ImGui::EndMenu();
            }


            // ----------------------------------------------------
            // Run
            // ----------------------------------------------------

            if (ImGui::BeginMenu("Run"))
            {
                const bool canRun =
                    !commandRunning.load();


                if (ImGui::MenuItem(
                    "Run Game",
                    nullptr,
                    false,
                    canRun))
                {
                    StartCommand(
                        "runGame.bat"
                    );
                }

                if (ImGui::MenuItem(
                    "Run Model Maker",
                    nullptr,
                    false,
                    canRun))
                {
                    StartCommand(
                        "runModelMaker.bat"
                    );
                }

                if (ImGui::MenuItem(
                    "Run Level Editor",
                    nullptr,
                    false,
                    canRun))
                {
                    StartCommand(
                        "runLevelEditor.bat"
                    );
                }


                ImGui::EndMenu();
            }


            // ----------------------------------------------------
            // Quit
            // ----------------------------------------------------

            if (ImGui::BeginMenu("Quit"))
            {
                if (ImGui::MenuItem("Quit"))
                {
                    quitRequested = true;
                }

                ImGui::EndMenu();
            }


            ImGui::EndMainMenuBar();
        }


        // ========================================================
        // About window
        // ========================================================

        if (showAbout)
        {
            ImGui::Begin(
                "About",
                &showAbout
            );


            ImGui::Text(
                "%s",
                currentVersion.c_str()
            );


            ImGui::Spacing();


            ImGui::TextWrapped(
                "This is something I made to make compiling "
                "my projects easier!"
            );


            ImGui::Spacing();


            ImGui::Text(
                "TimWorks Engine Builder"
            );


            ImGui::End();
        }


        // ========================================================
        // Console
        // ========================================================

        ImGui::SetNextWindowPos(
            ImVec2(0, 25),
            ImGuiCond_Always
        );


        ImGui::SetNextWindowSize(
            ImVec2(
                static_cast<float>(GetScreenWidth()),
                static_cast<float>(GetScreenHeight() - 25)
            ),
            ImGuiCond_Always
        );


        ImGui::Begin(
            "Console",
            nullptr,
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse
        );


        // --------------------------------------------------------
        // Console controls
        // --------------------------------------------------------

        ImGui::Checkbox(
            "Auto-scroll",
            &consoleAutoScroll
        );


        ImGui::SameLine();


        if (commandRunning.load())
        {
            ImGui::Text(
                "Building..."
            );
        }
        else
        {
            ImGui::Text(
                "Ready"
            );
        }


        ImGui::Separator();


        // --------------------------------------------------------
        // Console output
        // --------------------------------------------------------

        ImGui::BeginChild(
            "ConsoleOutput",
            ImVec2(0, 0),
            true,
            ImGuiWindowFlags_HorizontalScrollbar
        );


        {
            std::lock_guard<std::mutex> lock(
                consoleMutex
            );


            if (!consoleOutput.empty())
            {
                ImGui::TextUnformatted(
                    consoleOutput.c_str()
                );


                if (consoleAutoScroll &&
                    ImGui::GetScrollY() >=
                    ImGui::GetScrollMaxY() - 20.0f)
                {
                    ImGui::SetScrollHereY(1.0f);
                }
            }
        }


        ImGui::EndChild();


        ImGui::End();


        // ========================================================
        // End frame
        // ========================================================

        rlImGuiEnd();

        EndDrawing();


        // ========================================================
        // Quit after frame is finished
        // ========================================================

        if (quitRequested)
        {
            break;
        }
    }


    // ============================================================
    // Shutdown
    // ============================================================

    deInit();

    return 0;
}