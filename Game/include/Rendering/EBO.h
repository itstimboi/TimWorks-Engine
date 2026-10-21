#pragma once

#include <vector>

#include <glad/glad.h>

class EBO
{
public:

    unsigned int ID = 0;

    EBO(std::vector<unsigned int>& indices);

    ~EBO();

    EBO(const EBO&) = delete;
    EBO& operator=(const EBO&) = delete;

    void Bind();
    void Unbind();

    void Delete();
};