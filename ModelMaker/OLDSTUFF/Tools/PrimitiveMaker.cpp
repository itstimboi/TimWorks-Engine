#include "PrimitiveMaker.h"

#include <cmath>

namespace TW
{

static ModelVertex MakeVertex(
    Vector3 position,
    Vector3 normal,
    Vector2 texcoord
)
{
    ModelVertex vertex;

    vertex.position = position;
    vertex.normal = normal;
    vertex.color = WHITE;
    vertex.texcoord = texcoord;

    return vertex;
}


// ============================================================
// Cube
// ============================================================

ModelFrame CreateCubeFrame(
    float width,
    float height,
    float depth
)
{
    ModelFrame frame;

    float x = width * 0.5f;
    float y = height * 0.5f;
    float z = depth * 0.5f;

    // Each face gets its own vertices so that normals,
    // UVs, and materials can be edited independently.

    struct Face
    {
        Vector3 normal;
        Vector3 corners[4];
    };

    Face faces[6] =
    {
        // Front
        {
            { 0, 0, 1 },
            {
                { -x, -y, z },
                {  x, -y, z },
                {  x,  y, z },
                { -x,  y, z }
            }
        },

        // Back
        {
            { 0, 0, -1 },
            {
                {  x, -y, -z },
                { -x, -y, -z },
                { -x,  y, -z },
                {  x,  y, -z }
            }
        },

        // Left
        {
            { -1, 0, 0 },
            {
                { -x, -y, -z },
                { -x, -y,  z },
                { -x,  y,  z },
                { -x,  y, -z }
            }
        },

        // Right
        {
            { 1, 0, 0 },
            {
                { x, -y, z },
                { x, -y, -z },
                { x, y, -z },
                { x, y, z }
            }
        },

        // Top
        {
            { 0, 1, 0 },
            {
                { -x, y, z },
                { x, y, z },
                { x, y, -z },
                { -x, y, -z }
            }
        },

        // Bottom
        {
            { 0, -1, 0 },
            {
                { -x, -y, -z },
                { x, -y, -z },
                { x, -y, z },
                { -x, -y, z }
            }
        }
    };

    for (int face = 0; face < 6; face++)
    {
        unsigned int firstIndex =
            static_cast<unsigned int>(
                frame.indices.size()
            );

        unsigned int firstVertex =
            static_cast<unsigned int>(
                frame.vertices.size()
            );

        frame.vertices.push_back(
            MakeVertex(
                faces[face].corners[0],
                faces[face].normal,
                { 0, 1 }
            )
        );

        frame.vertices.push_back(
            MakeVertex(
                faces[face].corners[1],
                faces[face].normal,
                { 1, 1 }
            )
        );

        frame.vertices.push_back(
            MakeVertex(
                faces[face].corners[2],
                faces[face].normal,
                { 1, 0 }
            )
        );

        frame.vertices.push_back(
            MakeVertex(
                faces[face].corners[3],
                faces[face].normal,
                { 0, 0 }
            )
        );

        frame.indices.push_back(firstVertex + 0);
        frame.indices.push_back(firstVertex + 1);
        frame.indices.push_back(firstVertex + 2);

        frame.indices.push_back(firstVertex + 2);
        frame.indices.push_back(firstVertex + 3);
        frame.indices.push_back(firstVertex + 0);

        ModelSurface surface;

        surface.firstIndex = firstIndex;
        surface.indexCount = 6;
        surface.material = -1;
        surface.shading = SurfaceShading::Flat;

        frame.surfaces.push_back(surface);
    }

    return frame;
}


// ============================================================
// Plane
// ============================================================

ModelFrame CreatePlaneFrame(
    float width,
    float depth
)
{
    ModelFrame frame;

    float x = width * 0.5f;
    float z = depth * 0.5f;

    frame.vertices =
    {
        MakeVertex(
            { -x, 0, -z },
            { 0, 1, 0 },
            { 0, 1 }
        ),

        MakeVertex(
            {  x, 0, -z },
            { 0, 1, 0 },
            { 1, 1 }
        ),

        MakeVertex(
            {  x, 0,  z },
            { 0, 1, 0 },
            { 1, 0 }
        ),

        MakeVertex(
            { -x, 0,  z },
            { 0, 1, 0 },
            { 0, 0 }
        )
    };

    frame.indices =
    {
        0, 1, 2,
        2, 3, 0
    };

    ModelSurface surface;

    surface.firstIndex = 0;
    surface.indexCount = 6;
    surface.material = -1;
    surface.shading = SurfaceShading::Smooth;

    frame.surfaces.push_back(surface);

    return frame;
}


// ============================================================
// Sphere
// ============================================================

ModelFrame CreateSphereFrame(
    float radius,
    int rings,
    int segments
)
{
    ModelFrame frame;

    if (rings < 2)
        rings = 2;

    if (segments < 3)
        segments = 3;

    // const float PI = 3.14159265358979323846f;

    for (int ring = 0; ring <= rings; ring++)
    {
        float v =
            static_cast<float>(ring) /
            static_cast<float>(rings);

        float phi = v * PI;

        float y = std::cos(phi);
        float sinPhi = std::sin(phi);

        for (int segment = 0;
             segment <= segments;
             segment++)
        {
            float u =
                static_cast<float>(segment) /
                static_cast<float>(segments);

            float theta =
                u * PI * 2.0f;

            Vector3 normal =
            {
                sinPhi * std::cos(theta),
                y,
                sinPhi * std::sin(theta)
            };

            Vector3 position =
            {
                normal.x * radius,
                normal.y * radius,
                normal.z * radius
            };

            frame.vertices.push_back(
                MakeVertex(
                    position,
                    normal,
                    { u, v }
                )
            );
        }
    }

    for (int ring = 0; ring < rings; ring++)
    {
        for (int segment = 0;
             segment < segments;
             segment++)
        {
            unsigned int a =
                ring * (segments + 1) + segment;

            unsigned int b = a + 1;

            unsigned int c =
                (ring + 1) * (segments + 1) + segment;

            unsigned int d = c + 1;

            frame.indices.push_back(a);
            frame.indices.push_back(c);
            frame.indices.push_back(b);

            frame.indices.push_back(b);
            frame.indices.push_back(c);
            frame.indices.push_back(d);
        }
    }

    ModelSurface surface;

    surface.firstIndex = 0;
    surface.indexCount =
        static_cast<unsigned int>(
            frame.indices.size()
        );

    surface.material = -1;
    surface.shading = SurfaceShading::Smooth;

    frame.surfaces.push_back(surface);

    return frame;
}


// ============================================================
// Cylinder
// ============================================================

ModelFrame CreateCylinderFrame(
    float radius,
    float height,
    int segments
)
{
    ModelFrame frame;

    if (segments < 3)
        segments = 3;

    // const float PI = 3.14159265358979323846f;

    float halfHeight = height * 0.5f;

    // Side vertices

    for (int i = 0; i <= segments; i++)
    {
        float u =
            static_cast<float>(i) /
            static_cast<float>(segments);

        float angle =
            u * PI * 2.0f;

        float c = std::cos(angle);
        float s = std::sin(angle);

        Vector3 normal =
        {
            c,
            0,
            s
        };

        frame.vertices.push_back(
            MakeVertex(
                { c * radius, -halfHeight, s * radius },
                normal,
                { u, 1 }
            )
        );

        frame.vertices.push_back(
            MakeVertex(
                { c * radius, halfHeight, s * radius },
                normal,
                { u, 0 }
            )
        );
    }

    for (int i = 0; i < segments; i++)
    {
        unsigned int base =
            static_cast<unsigned int>(i * 2);

        frame.indices.push_back(base);
        frame.indices.push_back(base + 1);
        frame.indices.push_back(base + 2);

        frame.indices.push_back(base + 2);
        frame.indices.push_back(base + 1);
        frame.indices.push_back(base + 3);
    }

    // Bottom

    unsigned int bottomCenter =
        static_cast<unsigned int>(
            frame.vertices.size()
        );

    frame.vertices.push_back(
        MakeVertex(
            { 0, -halfHeight, 0 },
            { 0, -1, 0 },
            { 0.5f, 0.5f }
        )
    );

    unsigned int bottomStart =
        static_cast<unsigned int>(
            frame.vertices.size()
        );

    for (int i = 0; i <= segments; i++)
    {
        float u =
            static_cast<float>(i) /
            static_cast<float>(segments);

        float angle =
            u * PI * 2.0f;

        float c = std::cos(angle);
        float s = std::sin(angle);

        frame.vertices.push_back(
            MakeVertex(
                { c * radius, -halfHeight, s * radius },
                { 0, -1, 0 },
                {
                    c * 0.5f + 0.5f,
                    s * 0.5f + 0.5f
                }
            )
        );
    }

    for (int i = 0; i < segments; i++)
    {
        frame.indices.push_back(bottomCenter);
        frame.indices.push_back(bottomStart + i + 1);
        frame.indices.push_back(bottomStart + i);
    }

    // Top

    unsigned int topCenter =
        static_cast<unsigned int>(
            frame.vertices.size()
        );

    frame.vertices.push_back(
        MakeVertex(
            { 0, halfHeight, 0 },
            { 0, 1, 0 },
            { 0.5f, 0.5f }
        )
    );

    unsigned int topStart =
        static_cast<unsigned int>(
            frame.vertices.size()
        );

    for (int i = 0; i <= segments; i++)
    {
        float u =
            static_cast<float>(i) /
            static_cast<float>(segments);

        float angle =
            u * PI * 2.0f;

        float c = std::cos(angle);
        float s = std::sin(angle);

        frame.vertices.push_back(
            MakeVertex(
                { c * radius, halfHeight, s * radius },
                { 0, 1, 0 },
                {
                    c * 0.5f + 0.5f,
                    s * 0.5f + 0.5f
                }
            )
        );
    }

    for (int i = 0; i < segments; i++)
    {
        frame.indices.push_back(topCenter);
        frame.indices.push_back(topStart + i);
        frame.indices.push_back(topStart + i + 1);
    }

    ModelSurface surface;

    surface.firstIndex = 0;

    surface.indexCount =
        static_cast<unsigned int>(
            frame.indices.size()
        );

    surface.material = -1;
    surface.shading = SurfaceShading::Smooth;

    frame.surfaces.push_back(surface);

    return frame;
}


// ============================================================
// Cone
// ============================================================

ModelFrame CreateConeFrame(
    float radius,
    float height,
    int segments
)
{
    ModelFrame frame;

    if (segments < 3)
        segments = 3;

    // const float PI = 3.14159265358979323846f;

    float halfHeight = height * 0.5f;

    // Apex

    Vector3 apex =
    {
        0,
        halfHeight,
        0
    };

    // Sides

    for (int i = 0; i <= segments; i++)
    {
        float u =
            static_cast<float>(i) /
            static_cast<float>(segments);

        float angle =
            u * PI * 2.0f;

        float c = std::cos(angle);
        float s = std::sin(angle);

        Vector3 normal =
        {
            c,
            radius / height,
            s
        };

        float length =
            std::sqrt(
                normal.x * normal.x +
                normal.y * normal.y +
                normal.z * normal.z
            );

        normal.x /= length;
        normal.y /= length;
        normal.z /= length;

        frame.vertices.push_back(
            MakeVertex(
                { c * radius, -halfHeight, s * radius },
                normal,
                { u, 1 }
            )
        );

        frame.vertices.push_back(
            MakeVertex(
                apex,
                normal,
                { u, 0 }
            )
        );
    }

    for (int i = 0; i < segments; i++)
    {
        unsigned int base =
            static_cast<unsigned int>(i * 2);

        frame.indices.push_back(base);
        frame.indices.push_back(base + 1);
        frame.indices.push_back(base + 2);
    }

    // Bottom

    unsigned int center =
        static_cast<unsigned int>(
            frame.vertices.size()
        );

    frame.vertices.push_back(
        MakeVertex(
            { 0, -halfHeight, 0 },
            { 0, -1, 0 },
            { 0.5f, 0.5f }
        )
    );

    unsigned int start =
        static_cast<unsigned int>(
            frame.vertices.size()
        );

    for (int i = 0; i <= segments; i++)
    {
        float u =
            static_cast<float>(i) /
            static_cast<float>(segments);

        float angle =
            u * PI * 2.0f;

        float c = std::cos(angle);
        float s = std::sin(angle);

        frame.vertices.push_back(
            MakeVertex(
                {
                    c * radius,
                    -halfHeight,
                    s * radius
                },
                { 0, -1, 0 },
                {
                    c * 0.5f + 0.5f,
                    s * 0.5f + 0.5f
                }
            )
        );
    }

    for (int i = 0; i < segments; i++)
    {
        frame.indices.push_back(center);
        frame.indices.push_back(start + i + 1);
        frame.indices.push_back(start + i);
    }

    ModelSurface surface;

    surface.firstIndex = 0;

    surface.indexCount =
        static_cast<unsigned int>(
            frame.indices.size()
        );

    surface.material = -1;
    surface.shading = SurfaceShading::Smooth;

    frame.surfaces.push_back(surface);

    return frame;
}

}