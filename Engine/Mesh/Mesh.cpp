/*// #include "Mesh.h"

// Mesh::Mesh(
//     std::vector <Vertex>& vertices, 
//     std::vector <unsigned int>& indices, 
//     std::vector <Texture>& textures,
//     unsigned int instancing,
//     std::vector <glm::mat4> instanceMatrix
// )
// {
//     Mesh::vertices = vertices;
//     Mesh::indices = indices;
//     Mesh::textures = textures;
//     Mesh::instancing = instancing;

//     VAO.Bind();

//     VBO instanceVBO(instanceMatrix);
//     VBO VBO(vertices);
//     EBO EBO(indices);

//     VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
//     VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
//     VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));
//     VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texUV));

//     if (instancing == 1)
//     {
//         instanceVBO.Bind();

//         // VAO.LinkAttrib(instanceVBO, 4, 4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
//         // VAO.LinkAttrib(instanceVBO, 5, 4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
//         // VAO.LinkAttrib(instanceVBO, 6, 4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));
//         // VAO.LinkAttrib(instanceVBO, 7, 4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texUV));

//         // glVertexAttribDivisor(4, 1);
//         // glVertexAttribDivisor(5, 1);
//         // glVertexAttribDivisor(6, 1);
//         // glVertexAttribDivisor(7, 1);

//         glBindBuffer(GL_ARRAY_BUFFER, instanceVBO.ID);

//         glEnableVertexAttribArray(4);
//         glVertexAttribPointer(
//             4,
//             4,
//             GL_FLOAT,
//             GL_FALSE,
//             sizeof(glm::mat4),
//             (void*)0
//         );

//         glEnableVertexAttribArray(5);
//         glVertexAttribPointer(
//             5,
//             4,
//             GL_FLOAT,
//             GL_FALSE,
//             sizeof(glm::mat4),
//             (void*)(sizeof(glm::vec4))
//         );

//         glEnableVertexAttribArray(6);
//         glVertexAttribPointer(
//             6,
//             4,
//             GL_FLOAT,
//             GL_FALSE,
//             sizeof(glm::mat4),
//             (void*)(2 * sizeof(glm::vec4))
//         );

//         glEnableVertexAttribArray(7);
//         glVertexAttribPointer(
//             7,
//             4,
//             GL_FLOAT,
//             GL_FALSE,
//             sizeof(glm::mat4),
//             (void*)(3 * sizeof(glm::vec4))
//         );

//         glVertexAttribDivisor(4, 1);
//         glVertexAttribDivisor(5, 1);
//         glVertexAttribDivisor(6, 1);
//         glVertexAttribDivisor(7, 1);
//     }

//     VAO.Unbind();
//     VBO.Unbind();
//     instanceVBO.Unbind();
//     EBO.Unbind();
// }

// void Mesh::Draw
// (
// 	Shader& shader, 
// 	Camera& camera,
// 	glm::mat4 matrix,
// 	glm::vec3 translation, 
// 	glm::quat rotation, 
// 	glm::vec3 scale
// )
// {
//     shader.Activate();
//     VAO.Bind();

//     unsigned int numDiffuse = 0;
//     //unsigned int numSpecular = 0;

//     for (unsigned int i = 0; i < textures.size(); i++)
//     {
//         std::string num;
//         std::string type = textures[i].type;
        
//         // if (type == "diffuse")
//         // {
//         //     num = std::to_string(numDiffuse++);
//         // }
//         // else if (type == "specular")
//         // {
//         //     num = std::to_string(numSpecular++);
//         // }
    
//         num = std::to_string(numDiffuse++);

//         textures[i].texUnit(shader, (type + num).c_str(), i);
//         textures[i].Bind();
//     }

//     glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
//     camera.Matrix(shader, "camMatrix");

//     if (instancing == 1)
//     {
//         // Initialize matrices
//         glm::mat4 trans = glm::mat4(1.0f);
//         glm::mat4 rot = glm::mat4(1.0f);
//         glm::mat4 sca = glm::mat4(1.0f);

//         // Transform the matrices to their correct form
//         trans = glm::translate(trans, translation);
//         rot = glm::mat4_cast(rotation);
//         sca = glm::scale(sca, scale);

//         // Push the matrices to the vertex shader
//         glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
//         glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
//         glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
//         glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

//         glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//     }
//     else
//     {
//         glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instancing);
//     }
// }*/

#include "Mesh.h"

#include <glad/glad46.h>

namespace TWE
{
    static const char* vsS = R"GLSL(
        #version 110

        // Positions / Coordinates
        attribute vec3 aPos;

        // Normals
        attribute vec3 aNormal;

        // Colors
        attribute vec3 aColor;

        // Texture Coordinates
        attribute vec2 aTex;


        // Outputs to the Fragment Shader
        varying vec3 crntPos;
        varying vec3 Normal;
        varying vec3 color;
        varying vec2 texCoord;


        // Camera matrix
        uniform mat4 camMatrix;

        // Model matrix
        uniform mat4 model;

        uniform mat4 translation;
        uniform mat4 rotation;
        uniform mat4 scale;


        void main()
        {
            // Calculate current position
            crntPos =
                vec3(
                    model *
                    translation *
                    rotation *
                    scale *
                    vec4(aPos, 1.0)
                );

            // Normal
            Normal = aNormal;

            // Color
            color = aColor;

            // Texture coordinates
            texCoord = vec2(
                aTex.x,
                1.0 - aTex.y
            );

            // Final vertex position
            gl_Position =
                camMatrix *
                model *
                vec4(aPos, 1.0);
        }
        )GLSL";

    static const char* fsS = R"GLSL(
        #version 110


        // Current position from Vertex Shader
        varying vec3 crntPos;

        // Normal from Vertex Shader
        varying vec3 Normal;

        // Color from Vertex Shader
        varying vec3 color;

        // Texture coordinates from Vertex Shader
        varying vec2 texCoord;


        // Diffuse texture
        uniform sampler2D diffuse0;

        // Color of the light
        uniform vec4 lightColor;

        // Camera position
        uniform vec3 camPos;

        // Whether lighting is enabled
        uniform bool usingLights;


        // ============================================================
        // Point Light
        // ============================================================

        struct PointLight
        {
            vec3 position;

            float constant;
            float linear;
            float quadratic;

            vec3 ambient;
            vec3 diffuse;

            float specular;
        };


        #define NR_POINT_LIGHTS 16

        uniform PointLight pointLights[NR_POINT_LIGHTS];


        // ============================================================
        // Point Light Function
        // ============================================================

        vec4 pointLight(PointLight light)
        {
            vec3 lightVec =
                light.position - crntPos;

            float dist =
                length(lightVec);

            float inten =
                1.0 /
                (
                    light.constant +
                    light.linear * dist +
                    light.quadratic * dist * dist
                );

            vec3 normal =
                normalize(Normal);

            vec3 lightDirection =
                normalize(light.position - crntPos);

            float diffuse =
                max(
                    dot(normal, lightDirection),
                    0.0
                );

            vec4 texColor =
                texture2D(
                    diffuse0,
                    texCoord
                );

            vec3 lighting =
                light.ambient +
                light.diffuse *
                diffuse *
                inten;

            return vec4(
                texColor.rgb * lighting,
                texColor.a
            );
        }


        // ============================================================
        // Directional Light
        // ============================================================

        vec4 direcLight()
        {
            // Ambient lighting
            float ambient = 0.20;

            // Diffuse lighting
            vec3 normal =
                normalize(Normal);

            vec3 lightDirection =
                normalize(
                    vec3(
                        1.0,
                        1.0,
                        0.0
                    )
                );

            float diffuse =
                max(
                    dot(normal, lightDirection),
                    0.0
                );

            // Specular lighting
            float specularLight = 0.50;

            vec3 viewDirection =
                normalize(
                    camPos - crntPos
                );

            vec3 reflectionDirection =
                reflect(
                    -lightDirection,
                    normal
                );

            float specAmount =
                pow(
                    max(
                        dot(
                            viewDirection,
                            reflectionDirection
                        ),
                        0.0
                    ),
                    16.0
                );

            float specular =
                specAmount *
                specularLight;

            return
                (
                    texture2D(
                        diffuse0,
                        texCoord
                    )
                    *
                    (diffuse + ambient)
                )
                *
                lightColor;
        }


        // ============================================================
        // Spotlight
        // ============================================================

        struct SpotLight
        {
            vec3 position;
            vec3 direction;

            float constant;
            float linear;
            float quadratic;

            vec3 ambient;
            vec3 diffuse;

            float innerCone;
            float outerCone;
        };


        #define NR_SPOT_LIGHTS 16

        uniform SpotLight spotLights[NR_SPOT_LIGHTS];


        // ============================================================
        // Spotlight Function
        // ============================================================

        vec4 spotLight(SpotLight light)
        {
            vec3 lightVec =
                light.position - crntPos;

            float dist =
                length(lightVec);

            float inten =
                1.0 /
                (
                    light.constant +
                    light.linear * dist +
                    light.quadratic * dist * dist
                );

            vec3 normal =
                normalize(Normal);

            vec3 lightDirection =
                normalize(
                    light.position - crntPos
                );

            float diffuse =
                max(
                    dot(
                        normal,
                        lightDirection
                    ),
                    0.0
                );


            // --------------------------------------------------------
            // Spotlight cone
            // --------------------------------------------------------

            float angle =
                dot(
                    normalize(-light.direction),
                    normalize(-lightVec)
                );

            float coneIntensity =
                clamp(
                    (
                        angle -
                        light.outerCone
                    )
                    /
                    (
                        light.innerCone -
                        light.outerCone
                    ),
                    0.0,
                    1.0
                );


            // --------------------------------------------------------
            // Texture
            // --------------------------------------------------------

            vec4 texColor =
                texture2D(
                    diffuse0,
                    texCoord
                );


            // --------------------------------------------------------
            // Lighting
            // --------------------------------------------------------

            vec3 lighting =
                light.diffuse *
                diffuse *
                inten *
                coneIntensity;


            return vec4(
                texColor.rgb * lighting,
                texColor.a
            );
        }


        // ============================================================
        // Main
        // ============================================================

        void main()
        {
            vec4 texColor =
                texture2D(
                    diffuse0,
                    texCoord
                );


            // No lighting
            if (!usingLights)
            {
                gl_FragColor =
                    texColor;

                return;
            }


            // Base ambient lighting
            vec3 result =
                texColor.rgb *
                0.20;


            // --------------------------------------------------------
            // Point lights
            // --------------------------------------------------------

            for (int i = 0; i < NR_POINT_LIGHTS; i++)
            {
                result +=
                    pointLight(
                        pointLights[i]
                    ).rgb;
            }


            // --------------------------------------------------------
            // Spot lights
            // --------------------------------------------------------

            for (int i = 0; i < NR_SPOT_LIGHTS; i++)
            {
                result +=
                    spotLight(
                        spotLights[i]
                    ).rgb;
            }


            // Final color
            gl_FragColor =
                vec4(
                    result,
                    texColor.a
                )
                *
                lightColor;
        }
        )GLSL";

    static Shader meshShader(
            (ShaderSource)
            {
                vsS,
                fsS
            }
        );

    Mesh::Mesh(
        std::vector<Vertex> vertices,
        std::vector<unsigned int> indices,
        std::vector<Material> materials,
        unsigned int instancing,
        std::vector<glm::mat4> instanceMatrix
    )
        : vertices(std::move(vertices)),
        indices(std::move(indices)),
        materials(std::move(materials)),
        instancing(instancing)
    {
        // Mesh::vertices = vertices;
        // Mesh::indices = indices;
        // Mesh::materials = materials;
        // Mesh::instancing = instancing;

        VAO.Bind();

        VBO instanceVBO(instanceMatrix);
        VBO vertexVBO(vertices);
        EBO indexEBO(indices);

        VAO.LinkAttrib(
            vertexVBO,
            0,
            3,
            GL_FLOAT,
            sizeof(Vertex),
            (void*)offsetof(Vertex, position)
        );

        VAO.LinkAttrib(
            vertexVBO,
            1,
            3,
            GL_FLOAT,
            sizeof(Vertex),
            (void*)offsetof(Vertex, normal)
        );

        VAO.LinkAttrib(
            vertexVBO,
            2,
            3,
            GL_FLOAT,
            sizeof(Vertex),
            (void*)offsetof(Vertex, color)
        );

        VAO.LinkAttrib(
            vertexVBO,
            3,
            2,
            GL_FLOAT,
            sizeof(Vertex),
            (void*)offsetof(Vertex, texUV)
        );

        if (instancing != 1)
        {
            instanceVBO.Bind();

            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO.ID);

            glEnableVertexAttribArray(4);
            glVertexAttribPointer(
                4,
                4,
                GL_FLOAT,
                GL_FALSE,
                sizeof(glm::mat4),
                (void*)0
            );

            glEnableVertexAttribArray(5);
            glVertexAttribPointer(
                5,
                4,
                GL_FLOAT,
                GL_FALSE,
                sizeof(glm::mat4),
                (void*)(sizeof(glm::vec4))
            );

            glEnableVertexAttribArray(6);
            glVertexAttribPointer(
                6,
                4,
                GL_FLOAT,
                GL_FALSE,
                sizeof(glm::mat4),
                (void*)(2 * sizeof(glm::vec4))
            );

            glEnableVertexAttribArray(7);
            glVertexAttribPointer(
                7,
                4,
                GL_FLOAT,
                GL_FALSE,
                sizeof(glm::mat4),
                (void*)(3 * sizeof(glm::vec4))
            );

            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);
            glVertexAttribDivisor(7, 1);
        }

        VAO.Unbind();
        vertexVBO.Unbind();
        instanceVBO.Unbind();
        indexEBO.Unbind();
    }


    void Mesh::Draw(
        Camera& camera,
        glm::mat4 matrix,
        glm::vec3 translation,
        glm::quat rotation,
        glm::vec3 scale
    )
    {
        meshShader.Activate();
        VAO.Bind();
        
        for (unsigned int j = 0; j < materials.size(); j++)
        {
            unsigned int numDiffuse = 0;

            for (unsigned int i = 0; i < materials[j].textures.size(); i++)
            {
                std::string num;
                std::string type = materials[j].textures[i].type;

                num = std::to_string(numDiffuse++);

                materials[j].textures[i].texUnit(
                    meshShader,
                    (type + num).c_str(),
                    i
                );

                materials[j].textures[i].Bind();
            }
        }

        meshShader.SetVec3(
            "camPos",
            (glm::vec3){
            camera.Position.x,
            camera.Position.y,
            camera.Position.z}
        );

        camera.Matrix(meshShader, "camMatrix");

        if (instancing == 1)
        {
            glm::mat4 trans = glm::translate(
                glm::mat4(1.0f),
                translation
            );

            glm::mat4 rot = glm::mat4_cast(rotation);

            glm::mat4 sca = glm::scale(
                glm::mat4(1.0f),
                scale
            );

            meshShader.SetMat4("translation", trans);

            meshShader.SetMat4("rotation", rot);

            meshShader.SetMat4("scale", sca);

            meshShader.SetMat4("model", matrix);

            glDrawElements(
                GL_TRIANGLES,
                static_cast<GLsizei>(indices.size()),
                GL_UNSIGNED_INT,
                nullptr
            );
        }
        else
        {
            glDrawElementsInstanced(
                GL_TRIANGLES,
                static_cast<GLsizei>(indices.size()),
                GL_UNSIGNED_INT,
                nullptr,
                instancing
            );
        }
    }

}