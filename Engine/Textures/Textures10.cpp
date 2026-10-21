#include "textures.h"

#include <glad/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <cstdint>
#include <vector>

Texture::Texture(const std::vector<uint8_t>& imageData)
{
    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* pixels = stbi_load_from_memory(
        imageData.data(),
        static_cast<int>(imageData.size()),
        &width,
        &height,
        &channels,
        4
    );

    if (!pixels)
    {
        std::cerr << "Failed to decode image from PAK\n";
        return;
    }

    // OpenGL 1.0 does not support texture objects.
    // This uploads the image into the current texture state.
    glEnable(GL_TEXTURE_2D);

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels
    );

    stbi_image_free(pixels);
}

void Texture::Bind() const
{
    glEnable(GL_TEXTURE_2D);
}