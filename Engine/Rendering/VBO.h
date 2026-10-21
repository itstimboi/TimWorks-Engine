#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Rendering/Vertex.h"

namespace TWE
{
class VBO
{
public:

    unsigned int ID = 0;

    VBO(std::vector<TWE::Vertex>& vertices);
    VBO(std::vector<glm::mat4>& matrices);

    ~VBO();

    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;

    void Bind();
    void Unbind();

    void Delete();
};

};