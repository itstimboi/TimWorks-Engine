#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "Rendering/shaderClass.h"

namespace TWE
{

    class Camera
    {
        public:
            glm::vec3 Position;
            glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 camMatrix;

            bool firstClick = true;

            int width;
            int height;

            float speed = 0.01f;
            float sensitivity = 100.0f;

            Camera(int width, int height, glm::vec3 position);

            void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
            void Matrix(Shader& shader, const char* uniform);
            // void Inputs(GLFWwindow* window);
    };

};