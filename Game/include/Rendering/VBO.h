#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

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
};

class VBO
{
public:

    unsigned int ID = 0;

    VBO(std::vector<TE::Vertex>& vertices);
    VBO(std::vector<glm::mat4>& matrices);

    ~VBO();

    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;

    void Bind();
    void Unbind();

    void Delete();
};