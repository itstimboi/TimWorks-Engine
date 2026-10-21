#include "shaderClass.h"

#include "Graphics/Graphics.h"

#include <glad/glad46.h>

#include <glm/gtc/type_ptr.hpp>

#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>


namespace TWE
{

    // ============================================================
    // Backend
    // ============================================================

    struct Shader::Backend
    {
        unsigned int ID = 0;
    };


    // ============================================================
    // Read Shader File
    // ============================================================

    std::string get_file_contents(
        const char* filename
    )
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


    // ============================================================
    // OpenGL Error Checking
    // ============================================================

    static void CompileErrors(
        unsigned int shader,
        const char* type
    )
    {
        GLint success = GL_FALSE;

        char infoLog[1024] = {};


        if (std::strcmp(type, "PROGRAM") != 0)
        {
            glGetShaderiv(
                shader,
                GL_COMPILE_STATUS,
                &success
            );

            if (success == GL_FALSE)
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

            return;
        }


        glGetProgramiv(
            shader,
            GL_LINK_STATUS,
            &success
        );

        if (success == GL_FALSE)
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


    // ============================================================
    // Create OpenGL Shader
    // ============================================================

    // static unsigned int CreateOpenGLShaderProgram(
    //     const char* vertexFile,
    //     const char* fragmentFile
    // )
    // {
    //     const std::string vertexCode =
    //         get_file_contents(vertexFile);

    //     const std::string fragmentCode =
    //         get_file_contents(fragmentFile);


    //     const char* vertexSource =
    //         vertexCode.c_str();

    //     const char* fragmentSource =
    //         fragmentCode.c_str();


    //     // ========================================================
    //     // Vertex Shader
    //     // ========================================================

    //     const unsigned int vertexShader =
    //         glCreateShader(GL_VERTEX_SHADER);

    //     glShaderSource(
    //         vertexShader,
    //         1,
    //         &vertexSource,
    //         nullptr
    //     );

    //     glCompileShader(vertexShader);

    //     CompileErrors(
    //         vertexShader,
    //         "VERTEX"
    //     );


    //     // ========================================================
    //     // Fragment Shader
    //     // ========================================================

    //     const unsigned int fragmentShader =
    //         glCreateShader(GL_FRAGMENT_SHADER);

    //     glShaderSource(
    //         fragmentShader,
    //         1,
    //         &fragmentSource,
    //         nullptr
    //     );

    //     glCompileShader(fragmentShader);

    //     CompileErrors(
    //         fragmentShader,
    //         "FRAGMENT"
    //     );


    //     // ========================================================
    //     // Program
    //     // ========================================================

    //     const unsigned int program =
    //         glCreateProgram();

    //     glAttachShader(
    //         program,
    //         vertexShader
    //     );

    //     glAttachShader(
    //         program,
    //         fragmentShader
    //     );

    //     glLinkProgram(program);

    //     CompileErrors(
    //         program,
    //         "PROGRAM"
    //     );


    //     // Shader objects are no longer needed.

    //     glDeleteShader(vertexShader);
    //     glDeleteShader(fragmentShader);


    //     return program;
    // }

    static unsigned int CreateOpenGLShaderProgram(
        const char* vertexSource,
        const char* fragmentSource
    )
    {
        // ========================================================
        // Vertex Shader
        // ========================================================

        const unsigned int vertexShader =
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


        // ========================================================
        // Fragment Shader
        // ========================================================

        const unsigned int fragmentShader =
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


        // ========================================================
        // Program
        // ========================================================

        const unsigned int program =
            glCreateProgram();

        glAttachShader(
            program,
            vertexShader
        );

        glAttachShader(
            program,
            fragmentShader
        );

        glLinkProgram(program);

        CompileErrors(
            program,
            "PROGRAM"
        );


        // Shader objects are no longer needed.

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);


        return program;
    }

    // ============================================================
    // Constructor
    // ============================================================

    Shader::Shader(
        const char* vertexFile,
        const char* fragmentFile,
        ShaderLanguage language
    )
        : language(language)
    {
        // --------------------------------------------------------
        // Determine renderer
        // --------------------------------------------------------

        GraphicsRenderer renderer =
            GetGraphicsRenderer();


        // --------------------------------------------------------
        // Automatically determine shader language
        // --------------------------------------------------------

        if (this->language == ShaderLanguage::Auto)
        {
            switch (renderer.API)
            {
                case Renderer::GL:

                    this->language =
                        ShaderLanguage::GLSL;

                    break;


                case Renderer::DX:

                    this->language =
                        ShaderLanguage::HLSL;

                    break;


                case Renderer::VK:

                    this->language =
                        ShaderLanguage::GLSL;

                    break;


                default:

                    break;
            }
        }


        // --------------------------------------------------------
        // OpenGL
        // --------------------------------------------------------

        if (renderer.API == Renderer::GL)
        {
            if (
                this->language != ShaderLanguage::GLSL &&
                this->language != ShaderLanguage::Auto
            )
            {
                throw std::runtime_error(
                    "OpenGL renderer requires GLSL."
                );
            }


            // Read files

            const std::string vertexCode =
                get_file_contents(vertexFile);

            const std::string fragmentCode =
                get_file_contents(fragmentFile);


            // Create backend

            backend =
                new Backend();


            // Compile GLSL

            backend->ID =
                CreateOpenGLShaderProgram(
                    vertexCode.c_str(),
                    fragmentCode.c_str()
                );


            return;
        }


        // --------------------------------------------------------
        // DirectX
        // --------------------------------------------------------

        if (renderer.API == Renderer::DX)
        {
            throw std::runtime_error(
                "DirectX Shader backend is not implemented yet."
            );
        }


        // --------------------------------------------------------
        // Vulkan
        // --------------------------------------------------------

        if (renderer.API == Renderer::VK)
        {
            throw std::runtime_error(
                "Vulkan Shader backend is not implemented yet."
            );
        }


        // --------------------------------------------------------
        // Software
        // --------------------------------------------------------

        if (renderer.API == Renderer::Software)
        {
            throw std::runtime_error(
                "Software Shader backend is not implemented yet."
            );
        }


        throw std::runtime_error(
            "Graphics renderer does not support shaders."
        );
    }

    Shader::Shader(
        ShaderSource source,
        ShaderLanguage language
    )
        : language(language)
    {
        // --------------------------------------------------------
        // Determine renderer
        // --------------------------------------------------------

        GraphicsRenderer renderer =
            GetGraphicsRenderer();


        // --------------------------------------------------------
        // Automatically determine shader language
        // --------------------------------------------------------

        if (this->language == ShaderLanguage::Auto)
        {
            switch (renderer.API)
            {
                case Renderer::GL:

                    this->language =
                        ShaderLanguage::GLSL;

                    break;


                case Renderer::DX:

                    this->language =
                        ShaderLanguage::HLSL;

                    break;


                case Renderer::VK:

                    this->language =
                        ShaderLanguage::GLSL;

                    break;


                default:

                    break;
            }
        }


        // --------------------------------------------------------
        // OpenGL
        // --------------------------------------------------------

        if (renderer.API == Renderer::GL)
        {
            if (
                this->language != ShaderLanguage::GLSL &&
                this->language != ShaderLanguage::Auto
            )
            {
                throw std::runtime_error(
                    "OpenGL renderer requires GLSL."
                );
            }


            // Create backend

            backend =
                new Backend();


            // Compile directly from source

            backend->ID =
                CreateOpenGLShaderProgram(
                    source.vertex,
                    source.fragment
                );


            return;
        }


        // --------------------------------------------------------
        // DirectX
        // --------------------------------------------------------

        if (renderer.API == Renderer::DX)
        {
            throw std::runtime_error(
                "DirectX Shader backend is not implemented yet."
            );
        }


        // --------------------------------------------------------
        // Vulkan
        // --------------------------------------------------------

        if (renderer.API == Renderer::VK)
        {
            throw std::runtime_error(
                "Vulkan Shader backend is not implemented yet."
            );
        }


        // --------------------------------------------------------
        // Software
        // --------------------------------------------------------

        if (renderer.API == Renderer::Software)
        {
            throw std::runtime_error(
                "Software Shader backend is not implemented yet."
            );
        }


        throw std::runtime_error(
            "Graphics renderer does not support shaders."
        );
    }


    // ============================================================
    // Destructor
    // ============================================================

    Shader::~Shader()
    {
        Delete();
    }


    // ============================================================
    // Move Constructor
    // ============================================================

    Shader::Shader(
        Shader&& other
    ) noexcept
        : backend(other.backend),
          language(other.language)
    {
        other.backend = nullptr;
    }


    // ============================================================
    // Move Assignment
    // ============================================================

    Shader& Shader::operator=(
        Shader&& other
    ) noexcept
    {
        if (this != &other)
        {
            Delete();


            backend =
                other.backend;

            language =
                other.language;


            other.backend = nullptr;
        }

        return *this;
    }


    // ============================================================
    // Activate
    // ============================================================

    void Shader::Activate()
    {
        if (backend == nullptr)
            return;


        if (backend->ID != 0)
        {
            glUseProgram(
                backend->ID
            );
        }
    }


    // ============================================================
    // Delete
    // ============================================================

    void Shader::Delete()
    {
        if (backend == nullptr)
            return;


        if (backend->ID != 0)
        {
            glDeleteProgram(
                backend->ID
            );

            backend->ID = 0;
        }


        delete backend;

        backend = nullptr;
    }


    // ============================================================
    // Get Uniform Location
    // ============================================================

    static int GetUniformLocation(
        unsigned int program,
        const std::string& name
    )
    {
        return glGetUniformLocation(
            program,
            name.c_str()
        );
    }


    // ============================================================
    // SetBool
    // ============================================================

    void Shader::SetBool(
        const std::string& name,
        bool value
    )
    {
        if (backend == nullptr)
            return;


        glUniform1i(
            GetUniformLocation(
                backend->ID,
                name
            ),
            static_cast<int>(value)
        );
    }


    // ============================================================
    // SetInt
    // ============================================================

    void Shader::SetInt(
        const std::string& name,
        int value
    )
    {
        if (backend == nullptr)
            return;


        glUniform1i(
            GetUniformLocation(
                backend->ID,
                name
            ),
            value
        );
    }


    // ============================================================
    // SetFloat
    // ============================================================

    void Shader::SetFloat(
        const std::string& name,
        float value
    )
    {
        if (backend == nullptr)
            return;


        glUniform1f(
            GetUniformLocation(
                backend->ID,
                name
            ),
            value
        );
    }


    // ============================================================
    // SetVec3
    // ============================================================

    void Shader::SetVec3(
        const std::string& name,
        const glm::vec3& value
    )
    {
        if (backend == nullptr)
            return;


        glUniform3fv(
            GetUniformLocation(
                backend->ID,
                name
            ),
            1,
            glm::value_ptr(value)
        );
    }


    // ============================================================
    // SetVec4
    // ============================================================

    void Shader::SetVec4(
        const std::string& name,
        const glm::vec4& value
    )
    {
        if (backend == nullptr)
            return;


        glUniform4fv(
            GetUniformLocation(
                backend->ID,
                name
            ),
            1,
            glm::value_ptr(value)
        );
    }


    // ============================================================
    // SetMat4
    // ============================================================

    void Shader::SetMat4(
        const std::string& name,
        const glm::mat4& value
    )
    {
        if (backend == nullptr)
            return;


        glUniformMatrix4fv(
            GetUniformLocation(
                backend->ID,
                name
            ),
            1,
            GL_FALSE,
            glm::value_ptr(value)
        );
    }


    // ============================================================
    // Get Language
    // ============================================================

    ShaderLanguage Shader::GetLanguage() const
    {
        return language;
    }

}