#include "camera.h"

// #include <glad/gl.h>

#include <cmath>

Camera::Camera(GLFWwindow* window)
    : position(0.0f, 2.0f, 5.0f),
      window(window)
{
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);

    glfwSetInputMode(
        window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED
    );
}

void Camera::Update(float deltaTime)
{
    // Mouse look.
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (firstMouse)
    {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
    }

    float offsetX =
        static_cast<float>(lastMouseX - mouseX);

    float offsetY =
        static_cast<float>(lastMouseY - mouseY);

    lastMouseX = mouseX;
    lastMouseY = mouseY;

    yaw += offsetX * mouseSensitivity;
    pitch += offsetY * mouseSensitivity;

    if (pitch > 89.0f)
        pitch = 89.0f;

    if (pitch < -89.0f)
        pitch = -89.0f;

    // Convert yaw to a direction on the XZ plane.
    const float radians = 3.1415926535f / 180.0f;

    float angle = yaw * radians;

    glm::vec3 forward(
        -std::cos(angle),
        0.0f,
        std::sin(angle)
    );

    glm::vec3 right(
        std::sin(angle),
        0.0f,
        std::cos(angle)
    );

    float speed = movementSpeed * deltaTime;

    // WASD movement.
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += forward * speed;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= forward * speed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position -= right * speed;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position += right * speed;

    // Vertical movement.
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position.y += speed;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        position.y -= speed;
}

void Camera::ApplyProjection(float fov, float aspect)
{
    if (aspect <= 0.0f)
        return;

    const float nearPlane = 0.1f;
    const float farPlane = 1000.0f;
    const float radians = 3.1415926535f / 180.0f;

    float top = nearPlane *
        std::tan(fov * radians * 0.5f);

    float right = top * aspect;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(
        -right, right,
        -top, top,
        nearPlane, farPlane
    );

    glMatrixMode(GL_MODELVIEW);
}

void Camera::ApplyView()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Apply the inverse of the camera rotation.
    glRotatef(-pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(-(yaw + 90.0f), 0.0f, 1.0f, 0.0f);

    // Move the world in the opposite direction.
    glTranslatef(
        -position.x,
        -position.y,
        -position.z
    );
}