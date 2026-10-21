#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <json/json.hpp>

#include "Mesh.h"

using json = nlohmann::json;

namespace TWE
{
    using ModelHandle = uint32_t;

    enum class ModelDataEncoding
    {
        JSON,
        Base64
    };

    struct ModelBone
    {
        std::string name;
        int parent = -1;
        glm::mat4 transform = glm::mat4(1.0f);
    };

    struct ModelBonePose
    {
        std::string name;
        int parent = -1;

        glm::vec3 location = glm::vec3(0.0f);
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f);

        glm::mat4 transform = glm::mat4(1.0f);
        bool usesTRS = false;
    };

    struct ModelFrame
    {
        std::vector<ModelBonePose> bones;
    };

    struct AnimationKey
    {
        unsigned int frame = 0;
        float time = 0.0f;
        bool lerp = true;
    };

    struct ModelAnimation
    {
        std::string name;
        std::vector<AnimationKey> keys;
    };

    class Model
    {
    public:
        // Load from a .tw_mdl file.
        Model(
            const char* file,
            unsigned int instancing = 1,
            std::vector<glm::mat4> instanceMatrix = {}
        );

        // Load from raw JSON bytes.
        Model(
            const std::vector<unsigned char>& bytes,
            unsigned int instancing = 1,
            std::vector<glm::mat4> instanceMatrix = {}
        );

        // Load from JSON text or Base64-encoded JSON.
        Model(
            const std::string& data,
            ModelDataEncoding encoding,
            unsigned int instancing = 1,
            std::vector<glm::mat4> instanceMatrix = {}
        );

        ~Model();

        void Draw(
            Camera& camera,
            const glm::mat4& transform = glm::mat4(1.0f)
        );

        const std::vector<ModelBone>& GetBones() const;
        const std::vector<ModelFrame>& GetFrames() const;
        const std::vector<ModelAnimation>& GetAnimations() const;

        static bool Init();
        static void Shutdown();

        // File cache.
        static ModelHandle Pre_CacheModel(
            const std::string& path
        );

        // Raw JSON bytes cache.
        static ModelHandle Pre_CacheModel(
            const std::vector<unsigned char>& bytes
        );

        // Base64-encoded complete model cache.
        static ModelHandle Pre_CacheModelBase64(
            const std::string& base64
        );

        // JSON text cache.
        static ModelHandle Pre_CacheModelJSON(
            const std::string& jsonText
        );

        static void DrawModel(
            ModelHandle model,
            Camera& camera,
            const glm::mat4& transform = glm::mat4(1.0f)
        );

    private:
        std::string sourceKey;
        json JSON;

        unsigned int instancing = 1;
        std::vector<glm::mat4> instanceMatrix;

        std::vector<Mesh> meshes;
        std::vector<ModelBone> bones;
        std::vector<ModelFrame> frames;
        std::vector<ModelAnimation> animations;

        void loadFromJSONText(const std::string& jsonText);
        void loadMesh();
        void loadSkeleton();
        void loadAnimations();

        glm::mat4 readMatrix(const json& value) const;

        static std::vector<Model*> modelCache;
        static std::vector<std::string> modelCacheKeys;
    };
}