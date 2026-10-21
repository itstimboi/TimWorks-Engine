#pragma once

#include "Textures/Texture.h"
#include "Rendering/shaderClass.h"
#include "Camera/Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace TWE
{
    class Sprite
    {
    public:

        Sprite(
            const char* texturePath
        );

        ~Sprite();

        void Draw(
            glm::vec3 position,
            glm::vec3 scale,
            glm::quat rotation,
            TE::Shader& shader, 
            Camera& camera
        );

    private:

        Texture texture;

        glm::vec2 size;

        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
    };
};