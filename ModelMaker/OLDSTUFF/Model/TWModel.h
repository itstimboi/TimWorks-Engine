#pragma once

#include <string>
#include <vector>

#include <raylib/raylib.h>
#include <raylib/raymath.h>

#include <json/json.hpp>

namespace TW
{

struct ModelVertex
{
    Vector3 position = { 0, 0, 0 };
    Vector3 normal   = { 0, 1, 0 };

    Color color = WHITE;

    Vector2 texcoord = { 0, 0 };

    int boneIds[4] =
    {
        0, 0, 0, 0
    };

    float weights[4] =
    {
        1.0f, 0.0f, 0.0f, 0.0f
    };
};


struct ModelBone
{
    std::string name;

    int parent = -1;

    // Matrix transform = MatrixIdentity();

    Matrix transform = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
};


enum class SurfaceShading
{
    Flat,
    Smooth
};


struct ModelSurface
{
    unsigned int firstIndex = 0;

    unsigned int indexCount = 0;

    int material = -1;

    SurfaceShading shading =
        SurfaceShading::Smooth;
};


struct ModelFrame
{
    std::vector<ModelVertex> vertices;

    std::vector<unsigned int> indices;

    std::vector<ModelBone> bones;

    std::vector<ModelSurface> surfaces;
};


struct CollisionSurface
{
    unsigned int firstIndex = 0;

    unsigned int indexCount = 0;
};


struct CollisionData
{
    std::vector<Vector3> vertices;

    std::vector<unsigned int> indices;

    std::vector<CollisionSurface> surfaces;
};


struct EmbeddedTexture
{
    bool embedded = false;

    std::string mime;

    std::string data;
};


struct ModelMaterial
{
    std::string name;

    EmbeddedTexture texture;
};


struct AnimationKey
{
    int frame = 0;

    float time = 0.1f;

    bool lerp = true;
};


struct ModelAnimation
{
    std::string name;

    std::vector<AnimationKey> keys;
};


struct TWModel
{
    float version = 0.1f;

    std::vector<ModelMaterial> materials;

    ModelFrame idleFrame;

    CollisionData collision;

    std::vector<ModelFrame> frames;

    std::vector<ModelAnimation> animations;
};

}