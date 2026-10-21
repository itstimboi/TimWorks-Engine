#include "mesh.h"

#include <glad/gl.h>

std::vector<vertex> Mesh::GetVertices()
{
    return Vertices;
}

std::vector<unsigned int> Mesh::GetIndices()
{
    return Indices;
}

Mesh::Mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<Material> materials) : Vertices(vertices), Indices(indices), Materials(materials) {}

void Mesh::DrawMesh(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    // Save the current transformation.
    glPushMatrix();

    // Move the mesh.
    glTranslatef(
        position.x,
        position.y,
        position.z
    );

    // Rotate around each axis.
    glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);

    // Resize the mesh.
    glScalef(
        scale.x,
        scale.y,
        scale.z
    );

    // glEnable(GL_TEXTURE_2D);

    if (!Materials.empty() && Materials[0].tex)
    {
        Materials[0].tex->Bind();
    }

    glBegin(GL_TRIANGLES);

    for (unsigned int index : Indices)
    {
        if (index >= Vertices.size())
            continue;

        const vertex& v = Vertices[index];

        glColor3f(v.color.r, v.color.g, v.color.b);
        glNormal3f(v.normal.x, v.normal.y, v.normal.z);
        glTexCoord2f(v.texUV.x, v.texUV.y);

        glVertex3f(
            v.position.x,
            v.position.y,
            v.position.z
        );
    }

    glEnd();

    // Restore the previous transformation.
    glPopMatrix();
}