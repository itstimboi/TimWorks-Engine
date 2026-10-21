#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>


struct MapVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 uv;
};

struct MapEdge
{
    unsigned int vertexA = 0;
    unsigned int vertexB = 0;
};


struct MapEntity
{
    std::string type;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct MapMaterial
{
    std::string name;
    std::string texture;
};

struct MapSurface
{
    unsigned int material = -1;

    // Edges belonging to this surface.
    std::vector<unsigned int> edges;

    // Triangulation
    std::vector<unsigned int> indices;
};

struct MapData
{
    int version;

    std::string name;

    std::vector<MapMaterial> materials;

    std::vector<MapVertex> vertices;

    std::vector<MapEdge> edges;

    std::vector<MapSurface> surfaces;

    std::vector<MapEntity> entities;
};