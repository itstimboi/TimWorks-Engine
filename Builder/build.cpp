#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <algorithm>

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

std::string currentVersion = "App version: 0.0.6";


// ================================================================
// Console
// ================================================================

std::string consoleOutput;
std::mutex consoleMutex;

constexpr size_t MAX_CONSOLE_SIZE = 10 * 1024 * 1024; // 10 MB


// ================================================================
// Command threads
// ================================================================

std::vector<std::thread> commandThreads;
std::mutex commandMutex;

std::atomic<bool> commandRunning = false;


// ================================================================
// Help
// ================================================================

void PrintHelp(void)
{
    std::cout << "Available commands:\n";
    std::cout << "  help\n";
    std::cout << "  -help\n";
    std::cout << "  --help\n";
    std::cout << "  --version\n";
    std::cout << "  --buildEngine\n";
    std::cout << "  --buildLevelEditor\n";
    std::cout << "  --runEngine\n";
    std::cout << "  --runLevelEditor\n";
}


// ================================================================
// Add output safely
// ================================================================

void AppendConsoleOutput(const char* data, size_t size)
{
    std::lock_guard<std::mutex> lock(consoleMutex);

    consoleOutput.append(data, size);

    // Prevent unlimited memory growth.
    if (consoleOutput.size() > MAX_CONSOLE_SIZE)
    {
        size_t amountToRemove =
            consoleOutput.size() - MAX_CONSOLE_SIZE;

        consoleOutput.erase(
            0,
            amountToRemove
        );
    }
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

    if (!commandRunning.compare_exchange_strong(expected, true))
    {
        AppendConsoleOutput(
            "\n[Builder] A command is already running.\n",
            43
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
            "\n[Builder] Failed to create output pipe.\n",
            42
        );

        commandRunning = false;
        return;
    }


    // The parent must not inherit the read handle.

    SetHandleInformation(
        hRead,
        HANDLE_FLAG_INHERIT,
        0
    );


    // ------------------------------------------------------------
    // Create process
    // ------------------------------------------------------------

    STARTUPINFOA si{};

    PROCESS_INFORMATION pi{};

    si.cb = sizeof(STARTUPINFOA);

    si.dwFlags |= STARTF_USESTDHANDLES;

    si.hStdOutput = hWrite;
    si.hStdError = hWrite;


    // std::string cmdLine =
    //     "cmd.exe /C " + command;

    const std::string MSYS2_BASH =
        R"(E:\msys64\usr\bin\bash.exe)";

    std::string cmdLine;

    if (command.rfind("UCRT64:", 0) == 0)
    {
        std::string bashCommand =
            command.substr(7);

        cmdLine =
            "\"" + MSYS2_BASH + "\" -lc \"" +
            bashCommand +
            "\"";
    }
    else
    {
        cmdLine =
            "cmd.exe /C " + command;
    }

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
        AppendConsoleOutput(
            "\n[Builder] Failed to start command.\n",
            37
        );

        CloseHandle(hWrite);
        CloseHandle(hRead);

        commandRunning = false;

        return;
    }


    // ------------------------------------------------------------
    // Parent no longer needs the write handle
    // ------------------------------------------------------------

    CloseHandle(hWrite);
    hWrite = nullptr;


    // ------------------------------------------------------------
    // Read command output
    // ------------------------------------------------------------

    char buffer[4096];

    DWORD bytesRead = 0;

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


    // ------------------------------------------------------------
    // Wait for process
    // ------------------------------------------------------------

    WaitForSingleObject(
        pi.hProcess,
        INFINITE
    );


    // ------------------------------------------------------------
    // Cleanup process handles
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
    std::lock_guard<std::mutex> lock(commandMutex);

    commandThreads.emplace_back(
        RunCommand,
        command
    );
}


// ================================================================
// Join all command threads
// ================================================================

void JoinCommandThreads()
{
    std::lock_guard<std::mutex> lock(commandMutex);

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
    // Make sure background commands are finished
    // before destroying the GUI.

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


        if (cmd == "help" ||
            cmd == "-help" ||
            cmd == "--help")
        {
            PrintHelp();

            return 0;
        }


        else if (cmd == "--version")
        {
            std::cout
                << currentVersion
                << std::endl;

            return 0;
        }


        else if (cmd == "--buildEngine")
        {
            std::cout
                << "Building the Engine\n";

            return std::system(
                "buildEngine.bat"
            );
        }


        else if (cmd == "--buildLevelEditor")
        {
            std::cout
                << "Building the Level Editor\n";

            return std::system(
                "buildLevelEditor.bat"
            );
        }


        else if (cmd == "--runEngine")
        {
            std::cout
                << "Running the Engine\n";

            return std::system(
                "runEngine.bat"
            );
        }


        else if (cmd == "--runLevelEditor")
        {
            std::cout
                << "Running the Level Editor\n";

            return std::system(
                "runLevelEditor.bat"
            );
        }


        else
        {
            std::cout
                << "Unknown command.\n\n";

            PrintHelp();

            return 1;
        }
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
        "Builder"
    );

    rlImGuiSetup(true);


    // ============================================================
    // GUI state
    // ============================================================

    bool showAbout = false;

    bool quitRequested = false;


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
                if (ImGui::MenuItem(
                    "Build Engine",
                    nullptr,
                    false,
                    !commandRunning.load()))
                {
                    StartCommand(
                        "buildEngine.bat"
                    );
                }


                if (ImGui::MenuItem(
                    "Build Level Editor",
                    nullptr,
                    false,
                    !commandRunning.load()))
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
                if (ImGui::MenuItem(
                    "Run Engine",
                    nullptr,
                    false,
                    !commandRunning.load()))
                {
                    StartCommand(
                        "runEngine.bat"
                    );
                }


                if (ImGui::MenuItem(
                    "Run Level Editor",
                    nullptr,
                    false,
                    !commandRunning.load()))
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


            ImGui::Text(
                "This is Something I made to make compiling "
                "my projects easier!"
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
            ImVec2(1280, 695),
            ImGuiCond_Always
        );


        ImGui::Begin(
            "Console",
            nullptr,
            ImGuiWindowFlags_NoMove
        );


        {
            std::lock_guard<std::mutex> lock(
                consoleMutex
            );


            ImGui::BeginChild(
                "ConsoleOutput",
                ImVec2(0, 0),
                true,
                ImGuiWindowFlags_HorizontalScrollbar
            );


            ImGui::InputTextMultiline(
                "##ConsoleText",
                consoleOutput.data(),
                consoleOutput.size() + 1,
                ImVec2(-1, -1),
                ImGuiInputTextFlags_ReadOnly
            );


            ImGui::EndChild();
        }


        ImGui::End();


        // ========================================================
        // End frame
        // ========================================================

        rlImGuiEnd();

        EndDrawing();


        // ========================================================
        // Quit after the frame is completely finished
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