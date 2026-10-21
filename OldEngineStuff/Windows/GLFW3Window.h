#pragma once

#include "Windows.h"

#include <GLFW/glfw3.h>

class GLFW3Window : public Window
{
private:
    GLFWwindow* window = nullptr;

public:
    GLFW3Window();
    ~GLFW3Window() override;

    bool Create(
        int width,
        int height,
        const char* title
    ) override;

    void Destroy() override;

    void PollEvents() override;

    bool ShouldClose() const override;

    void SwapBuffers() override;

    void* GetNativeHandle() override;
};