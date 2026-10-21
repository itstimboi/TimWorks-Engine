#ifndef TEXTURES_H
#define TEXTURES_H

#include <vector>
#include <cstdint>

class Texture
{
public:
    Texture() = default;

    explicit Texture(const char* filename);

    Texture(const std::vector<uint8_t>& imageData);

    void Bind() const;

private:
    // OpenGL 1.0 has no texture object IDs.
    // Each Texture instance represents image data to upload.
    unsigned char* pixels = nullptr;

    int texW = 0;
    int texH = 0;
};

struct Material
{
    Texture* tex;

    explicit Material(Texture* texture)
        : tex(texture)
    {
    }
};

#endif