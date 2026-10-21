#pragma once

#include <string>

#include <glad/glad.h>

#include <glm/glm.hpp>

std::string ReadShaderFile(const char* filename);

class OpenGLShader
{
public:

    unsigned int ID = 0;

    OpenGLShader(
        const char* vertexFile,
        const char* fragmentFile
    );

    ~OpenGLShader();

    OpenGLShader(const OpenGLShader&) = delete;
    OpenGLShader& operator=(const OpenGLShader&) = delete;

    void Activate();
    void Delete();

    void SetBool(
        const std::string& name,
        bool value
    );

    void SetInt(
        const std::string& name,
        int value
    );

    void SetFloat(
        const std::string& name,
        float value
    );

    void SetVec3(
        const std::string& name,
        const glm::vec3& value
    );

    void SetVec4(
        const std::string& name,
        const glm::vec4& value
    );

    void SetMat4(
        const std::string& name,
        const glm::mat4& value
    );

private:

    void CompileErrors(
        unsigned int shader,
        const char* type
    );

    GLint GetUniformLocation(
        const std::string& name
    ) const;
};