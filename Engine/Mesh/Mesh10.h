#ifndef MESH_H

#define MESH_H

#include "Rendering/Vertex.h"
#include "Textures/Textures10.h"

#include <vector>

#include "Mesh.h"

namespace TWE
{
    class MeshGL10 : Mesh
    {

        public:

            std::vector<Vertex> GetVertices();
            std::vector<unsigned int> GetIndices();

            Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Material> materials);

            void DrawMesh(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

        private:
            std::vector<Vertex> Vertices;
            std::vector<unsigned int> Indices;
            std::vector<Material> Materials;
    };

};

#endif