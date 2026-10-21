#pragma once

#include <vector>

#include <glm/glm.hpp>

enum class LightType
{
    Directional,
    Point,
    Spot
};

struct Light
{
    LightType type;

    glm::vec3 position, direction, color;

    float intensity, constant, linear, quadratic, innerCutoff, outerCutoff;
};