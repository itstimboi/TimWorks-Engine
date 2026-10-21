
#include "Texture.h"

#include <glad/glad46.h>
#include "stb/stb_image.h"

#include <iostream>

namespace TWE
{
    static void UploadTexture(
        unsigned int& textureID,
        const unsigned char* bytes,
        int width,
        int height,
        int channels,
        unsigned int slot
    )
    {
        if (!bytes || width <= 0 || height <= 0)
        {
            std::cerr << "[TimWorks] Invalid texture image data.\n";
            return;
        }

        GLenum format = GL_RGB;

        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;
        else
        {
            std::cerr << "[TimWorks] Unsupported texture channel count.\n";
            return;
        }

        glGenTextures(1, &textureID);
        glActiveTexture(slot);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_REPEAT
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_REPEAT
        );

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            bytes
        );

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture::Texture(
        const char* image,
        const char* texType,
        unsigned int slot
    )
        : type(texType),
          unit(slot - GL_TEXTURE0)
    {
        int width = 0;
        int height = 0;
        int channels = 0;

        stbi_set_flip_vertically_on_load(true);

        unsigned char* bytes = stbi_load(
            image,
            &width,
            &height,
            &channels,
            0
        );

        if (!bytes)
        {
            std::cerr
                << "[TimWorks] Failed to load texture: "
                << (image ? image : "(null)")
                << "\n";

            return;
        }

        UploadTexture(
            ID,
            bytes,
            width,
            height,
            channels,
            slot
        );

        stbi_image_free(bytes);
    }

    Texture::Texture(
        const unsigned char* imageData,
        int imageSize,
        const char* texType,
        unsigned int slot
    )
        : type(texType),
          unit(slot - GL_TEXTURE0)
    {
        if (!imageData || imageSize <= 0)
        {
            std::cerr << "[TimWorks] Empty embedded texture.\n";
            return;
        }

        int width = 0;
        int height = 0;
        int channels = 0;

        stbi_set_flip_vertically_on_load(true);

        unsigned char* pixels = stbi_load_from_memory(
            imageData,
            imageSize,
            &width,
            &height,
            &channels,
            0
        );

        if (!pixels)
        {
            std::cerr
                << "[TimWorks] Failed to decode embedded texture: "
                << stbi_failure_reason()
                << "\n";

            return;
        }

        UploadTexture(
            ID,
            pixels,
            width,
            height,
            channels,
            slot
        );

        stbi_image_free(pixels);
    }

    void Texture::texUnit(
        Shader& shader,
        const char* uniform,
        unsigned int textureUnit
    )
    {
        shader.Activate();
        shader.SetInt(uniform, static_cast<int>(textureUnit));
    }

    void Texture::Bind()
    {
        if (ID == 0)
            return;

        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    void Texture::UnBind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Delete()
    {
        if (ID != 0)
        {
            glDeleteTextures(1, &ID);
            ID = 0;
        }
    }
}
