#include "OpenGLShader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>

#include <glad/glad46.h>

#include <glm/gtc/type_ptr.hpp>


std::string ReadShaderFile(const char* filename)
{
    errno = 0;

    std::ifstream in(
        filename,
        std::ios::binary
    );

    if (!in.is_open())
    {
        std::cerr
            << "Failed to open shader: "
            << filename
            << '\n';

        std::cerr
            << "errno = "
            << errno
            << '\n';

        std::cerr
            << "message = "
            << std::strerror(errno)
            << '\n';

        throw std::runtime_error(
            std::string("Failed to open shader: ")
            + filename
        );
    }

    std::ostringstream contents;

    contents << in.rdbuf();

    return contents.str();
}


OpenGLShader::OpenGLShader(
    const char* vertexFile,
    const char* fragmentFile
)
{
    std::string vertexCode =
        ReadShaderFile(vertexFile);

    std::string fragmentCode =
        ReadShaderFile(fragmentFile);

    const char* vertexSource =
        vertexCode.c_str();

    const char* fragmentSource =
        fragmentCode.c_str();


    //
    // Vertex shader
    //

    unsigned int vertexShader =
        glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexSource,
        nullptr
    );

    glCompileShader(vertexShader);

    CompileErrors(
        vertexShader,
        "VERTEX"
    );


    //
    // Fragment shader
    //

    unsigned int fragmentShader =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentSource,
        nullptr
    );

    glCompileShader(fragmentShader);

    CompileErrors(
        fragmentShader,
        "FRAGMENT"
    );


    //
    // Program
    //

    ID = glCreateProgram();

    glAttachShader(
        ID,
        vertexShader
    );

    glAttachShader(
        ID,
        fragmentShader
    );

    glLinkProgram(ID);

    CompileErrors(
        ID,
        "PROGRAM"
    );


    //
    // Shader objects are no longer needed
    //

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


OpenGLShader::~OpenGLShader()
{
    Delete();
}


void OpenGLShader::Activate()
{
    if (ID != 0)
        glUseProgram(ID);
}


void OpenGLShader::Delete()
{
    if (ID != 0)
    {
        glDeleteProgram(ID);
        ID = 0;
    }
}


void OpenGLShader::CompileErrors(
    unsigned int shader,
    const char* type
)
{
    GLint hasCompiled = GL_FALSE;

    char infoLog[1024] = {};


    if (std::strcmp(type, "PROGRAM") != 0)
    {
        glGetShaderiv(
            shader,
            GL_COMPILE_STATUS,
            &hasCompiled
        );

        if (hasCompiled == GL_FALSE)
        {
            glGetShaderInfoLog(
                shader,
                sizeof(infoLog),
                nullptr,
                infoLog
            );

            std::cerr
                << "SHADER_COMPILATION_ERROR for "
                << type
                << ":\n"
                << infoLog
                << '\n';
        }
    }
    else
    {
        glGetProgramiv(
            shader,
            GL_LINK_STATUS,
            &hasCompiled
        );

        if (hasCompiled == GL_FALSE)
        {
            glGetProgramInfoLog(
                shader,
                sizeof(infoLog),
                nullptr,
                infoLog
            );

            std::cerr
                << "SHADER_LINKING_ERROR:\n"
                << infoLog
                << '\n';
        }
    }
}


int OpenGLShader::GetUniformLocation(
    const std::string& name
) const
{
    return glGetUniformLocation(
        ID,
        name.c_str()
    );
}


void OpenGLShader::SetBool(
    const std::string& name,
    bool value
)
{
    glUniform1i(
        GetUniformLocation(name),
        static_cast<int>(value)
    );
}


void OpenGLShader::SetInt(
    const std::string& name,
    int value
)
{
    glUniform1i(
        GetUniformLocation(name),
        value
    );
}


void OpenGLShader::SetFloat(
    const std::string& name,
    float value
)
{
    glUniform1f(
        GetUniformLocation(name),
        value
    );
}


void OpenGLShader::SetVec3(
    const std::string& name,
    const glm::vec3& value
)
{
    glUniform3fv(
        GetUniformLocation(name),
        1,
        glm::value_ptr(value)
    );
}


void OpenGLShader::SetVec4(
    const std::string& name,
    const glm::vec4& value
)
{
    glUniform4fv(
        GetUniformLocation(name),
        1,
        glm::value_ptr(value)
    );
}


void OpenGLShader::SetMat4(
    const std::string& name,
    const glm::mat4& value
)
{
    glUniformMatrix4fv(
        GetUniformLocation(name),
        1,
        GL_FALSE,
        glm::value_ptr(value)
    );
}