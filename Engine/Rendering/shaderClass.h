#pragma once

#include <string>

#include <glm/glm.hpp>

namespace TWE
{
    enum class ShaderLanguage
    {
        Auto,

        GLSL,
        HLSL,
        DXAssembly
    };

    struct ShaderSource
    {
        const char* vertex;
        const char* fragment;
    };

    class Shader
    {
    public:

        Shader(
            const char* vertexFile,
            const char* fragmentFile,
            ShaderLanguage language = ShaderLanguage::Auto
        );

        Shader(
            ShaderSource source,
            ShaderLanguage language = ShaderLanguage::Auto
        );


        ~Shader();


        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;


        Shader(Shader&& other) noexcept;
        Shader& operator=(Shader&& other) noexcept;


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


        ShaderLanguage GetLanguage() const;


    private:

        struct Backend;

        Backend* backend = nullptr;

        ShaderLanguage language = ShaderLanguage::Auto;
    };


    std::string get_file_contents(
        const char* filename
    );
}