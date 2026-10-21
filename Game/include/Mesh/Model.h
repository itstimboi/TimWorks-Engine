#pragma once

#include <json/json.hpp>

#include "Mesh.h"

using json = nlohmann::json;

namespace TWE
{

    using ModelHandle = uint32_t;

    class Model
    {
    public:
        Model(
            const char* file,
            unsigned int instancing = 1,
            std::vector<glm::mat4> instanceMatrix = {}
        );

        void Draw(TE::Shader& shader, Camera& camera, const glm::mat4& transform = glm::mat4(1.0f));

        // Model cache
        static bool Init();
        static void Shutdown();

        static ModelHandle Pre_CacheModel(const std::string& path);

        static void DrawModel(
            ModelHandle model,
            TE::Shader& shader,
            Camera& camera,
            const glm::mat4& transform = glm::mat4(1.0f)
        );

    private:
        const char* file;

        std::vector<unsigned char> data;
        json JSON;

        unsigned int instancing;

        std::vector<Mesh> meshes;

        std::vector<glm::vec3> translationsMeshes;
        std::vector<glm::quat> rotationsMeshes;
        std::vector<glm::vec3> scalesMeshes;
        std::vector<glm::mat4> matricesMeshes;

        std::vector<glm::mat4> instanceMatrix;

        std::vector<std::string> loadedTexName;
        std::vector<Texture> loadedTex;

        void loadMesh(unsigned int indMesh);

        void traverseNode(
            unsigned int nextNode,
            glm::mat4 matrix = glm::mat4(1.0f)
        );

        std::vector<unsigned char> getData();

        std::vector<float> getFloats(json accessor);

        std::vector<unsigned int> getIndices(json accessor);

        std::vector<glm::ivec4> getJointIndices(json accessor);

        std::vector<Texture> getTextures(const json& primitive);

        std::vector<Vertex> assembleVertices(
            std::vector<glm::vec3> positions,
            std::vector<glm::vec3> normals,
            std::vector<glm::vec2> texUVs,
            std::vector<glm::ivec4> boneIDs,
            std::vector<glm::vec4> weights
        );

        std::vector<glm::vec2> groupFloatsVec2(
            std::vector<float> floatVec
        );

        std::vector<glm::vec3> groupFloatsVec3(
            std::vector<float> floatVec
        );

        std::vector<glm::vec4> groupFloatsVec4(
            std::vector<float> floatVec
        );

        // Cached models
        static std::vector<Model*> modelCache;
    };
    
};