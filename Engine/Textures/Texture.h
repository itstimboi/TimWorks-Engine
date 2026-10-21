
#pragma once

#include <vector>

#include "Rendering/shaderClass.h"

namespace TWE
{
    class Texture
    {
    public:
        unsigned int ID = 0;
        const char* type = "diffuse";
        unsigned int unit = 0;

        // Load a texture from a file.
        Texture(
            const char* image,
            const char* texType,
            unsigned int slot
        );

        // Load a texture from encoded image bytes in memory.
        Texture(
            const unsigned char* imageData,
            int imageSize,
            const char* texType,
            unsigned int slot
        );

        void texUnit(
            Shader& shader,
            const char* uniform,
            unsigned int unit
        );

        void Bind();
        void UnBind();
        void Delete();
    };

    struct Material
    {
        std::vector<Texture> textures;
    };
}
