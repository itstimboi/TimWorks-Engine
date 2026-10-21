#pragma once

#include "../Model/TWModel.h"

namespace TW
{

ModelFrame CreateCubeFrame(
    float width = 1.0f,
    float height = 1.0f,
    float depth = 1.0f
);

ModelFrame CreatePlaneFrame(
    float width = 1.0f,
    float depth = 1.0f
);

ModelFrame CreateSphereFrame(
    float radius = 0.5f,
    int rings = 16,
    int segments = 32
);

ModelFrame CreateCylinderFrame(
    float radius = 0.5f,
    float height = 1.0f,
    int segments = 32
);

ModelFrame CreateConeFrame(
    float radius = 0.5f,
    float height = 1.0f,
    int segments = 32
);

}