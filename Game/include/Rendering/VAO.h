#pragma once

#include <glad/glad.h>

#include "VBO.h"

class VAO
{
public:

    unsigned int ID = 0;

    VAO();

    ~VAO();

    VAO(const VAO&) = delete;
	VAO& operator=(const VAO&) = delete;

	VAO(VAO&& other) noexcept;
	VAO& operator=(VAO&& other) noexcept;

    void LinkAttrib(
        VBO& vbo,
        unsigned int layout,
        unsigned int numComponents,
        GLenum type,
        GLsizeiptr stride,
        void* offset
    );

    void Bind();
    void Unbind();

    void Delete();
};