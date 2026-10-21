#include "Sprite.h"

#include <glm/gtc/matrix_transform.hpp>

namespace TWE
{

    Sprite::Sprite(Material& material)
        : mesh(
            std::vector<Vertex>{
                Vertex{
                    glm::vec3(-0.5f, -0.5f, 0.0f),
                    glm::vec3(0.0f),
                    glm::vec3(1.0f),
                    glm::vec2(0.0f, 0.0f)
                },

                Vertex{
                    glm::vec3(0.5f, -0.5f, 0.0f),
                    glm::vec3(0.0f),
                    glm::vec3(1.0f),
                    glm::vec2(1.0f, 0.0f)
                },

                Vertex{
                    glm::vec3(0.5f, 0.5f, 0.0f),
                    glm::vec3(0.0f),
                    glm::vec3(1.0f),
                    glm::vec2(1.0f, 1.0f)
                },

                Vertex{
                    glm::vec3(-0.5f, 0.5f, 0.0f),
                    glm::vec3(0.0f),
                    glm::vec3(1.0f),
                    glm::vec2(0.0f, 1.0f)
                }
            },

            std::vector<unsigned int>{
                0, 1, 2,
                2, 3, 0
            },

            std::vector<Material>{
                material
            }
        )
    {
    }


    Sprite::~Sprite()
    {
    }


    void Sprite::Draw(
        glm::vec3 position,
        glm::vec3 scale,
        glm::quat rotation,
        Camera& camera
    )
    {
        mesh.Draw(
            camera,
            glm::mat4(1.0f),
            position,
            rotation,
            scale
        );
    }

}