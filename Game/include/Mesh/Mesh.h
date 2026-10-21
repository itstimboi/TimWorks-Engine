/*// #pragma once

// #include <string>

// #include "Rendering/VAO.h"
// #include "Rendering/EBO.h"

// #include "Camera/Camera.h"

// #include "Textures/Texture.h"

// class Mesh
// {
//     public:
//         std::vector <Vertex> vertices;
//         std::vector <unsigned int> indices;
//         std::vector <Texture> textures;

//         VAO VAO;

//         unsigned int instancing;

//         Mesh(
//             std::vector <Vertex>& vertices, 
//             std::vector <unsigned int>& indices, 
//             std::vector <Texture>& textures,
//             unsigned int instancing = 1,
//             std::vector <glm::mat4> instanceMatrix = {}
//         );

//         void Draw(
//             Shader& shader,
//             Camera& camera,
//             glm::mat4 matrix = glm::mat4(1.0f),
//             glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
//             glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
//             glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
//         );
// };*/

#pragma once

#include <vector>
#include <string>

#include "Rendering/VAO.h"
#include "Rendering/EBO.h"

#include "Camera/Camera.h"
#include "Textures/Texture.h"

namespace TWE
{
    class Mesh
    {
        public:

            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;

            VAO VAO;

            unsigned int instancing;

            Mesh(
                std::vector<Vertex>& vertices,
                std::vector<unsigned int>& indices,
                std::vector<Texture>& textures,
                unsigned int instancing = 1,
                std::vector<glm::mat4> instanceMatrix = {}
            );

            void Draw(
                TE::Shader& shader,
                Camera& camera,
                glm::mat4 matrix = glm::mat4(1.0f),
                glm::vec3 translation = glm::vec3(0.0f),
                glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                glm::vec3 scale = glm::vec3(1.0f)
            );
    };
};