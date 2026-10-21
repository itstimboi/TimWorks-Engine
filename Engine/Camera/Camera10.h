#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

class Camera
{
public:
    explicit Camera(GLFWwindow* window);

    void Update(float deltaTime);
    void ApplyProjection(float fov, float aspect);
    void ApplyView();

    glm::vec3 position;

    float movementSpeed = 5.0f;
    float mouseSensitivity = 0.1f;

    float yaw = -90.0f;
    float pitch = 0.0f;

private:
    GLFWwindow* window;

    double lastMouseX = 0.0;
    double lastMouseY = 0.0;

    bool firstMouse = true;
};

#endif