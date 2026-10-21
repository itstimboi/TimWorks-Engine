#include "VBO.h"

#include <cstddef>

#include <glad/glad46.h>

namespace TWE
{

VBO::VBO(std::vector<Vertex>& vertices)
{
    glGenBuffers(1, &ID);

    Bind();

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(
            vertices.size() * sizeof(Vertex)
        ),
        vertices.data(),
        GL_STATIC_DRAW
    );

    Unbind();
}

VBO::VBO(std::vector<glm::mat4>& matrices)
{
    glGenBuffers(1, &ID);

    Bind();

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(
            matrices.size() * sizeof(glm::mat4)
        ),
        matrices.data(),
        GL_STATIC_DRAW
    );

    Unbind();
}

VBO::~VBO()
{
    Delete();
}

void VBO::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
    if (ID != 0)
    {
        glDeleteBuffers(1, &ID);
        ID = 0;
    }
}

}