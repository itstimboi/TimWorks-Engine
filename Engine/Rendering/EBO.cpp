#include "EBO.h"

#include <glad/glad46.h>

namespace TWE
{

EBO::EBO(std::vector<unsigned int>& indices)
{
    glGenBuffers(1, &ID);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );
}

EBO::~EBO()
{
    Delete();
}

void EBO::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete()
{
    if (ID != 0)
    {
        glDeleteBuffers(1, &ID);
        ID = 0;
    }
}

}