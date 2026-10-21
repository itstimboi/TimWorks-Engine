#pragma once

#include "Textures/Texture.h"
#include "Rendering/shaderClass.h"
#include "Camera/Camera.h"
#include "Mesh/Mesh.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace TWE
{
    class Sprite
    {
    public:

        Sprite(Material& material);

        ~Sprite();

        void Draw(
            glm::vec3 position,
            glm::vec3 scale,
            glm::quat rotation,
            Camera& camera
        );

    private:
        Mesh mesh;
    };
};