#pragma once

#include "glm/glm.hpp"

namespace TWE
{

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal = glm::vec3(0.0f);
        glm::vec3 color;
        glm::vec2 texUV = glm::vec2(0.0f);

        glm::ivec4 boneIDs = glm::ivec4(0);
        glm::vec4 weights = glm::vec4(0.0f);
    };

}