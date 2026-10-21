#include "VAO.h"

#include <glad/glad46.h>

namespace TWE
{

VAO::VAO()
{
    glGenVertexArrays(1, &ID);
}

VAO::~VAO()
{
    Delete();
}

VAO::VAO(VAO&& other) noexcept
{
    ID = other.ID;
    other.ID = 0;
}

VAO& VAO::operator=(VAO&& other) noexcept
{
    if (this != &other)
    {
        Delete();

        ID = other.ID;
        
        other.ID = 0;
    }

    return *this;
}

void VAO::LinkAttrib(VBO &VBO, unsigned int layout, unsigned int numComponents, unsigned int type, signed long long int stride, void* offset)
{
    VBO.Bind();
    glVertexAttribPointer(layout, numComponents, GL_FLOAT, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    VBO.Unbind();
}

void VAO::Bind()
{
    glBindVertexArray(ID);
}

void VAO::Unbind()
{
    glBindVertexArray(0);
}

void VAO::Delete()
{
    glDeleteVertexArrays(1, &ID);
}

}