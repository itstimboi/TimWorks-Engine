#include "Model.h"

#include <glad/glad46.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace TWE
{
    // ============================================================
    // Static cache
    // ============================================================

    std::vector<Model*> Model::modelCache;
    std::vector<std::string> Model::modelCacheKeys;

    namespace
    {
        // ========================================================
        // JSON helpers
        // ========================================================

        bool ReadVec3(
            const json& value,
            glm::vec3& result
        )
        {
            if (!value.is_array() || value.size() < 3)
                return false;

            result = glm::vec3(
                value[0].get<float>(),
                value[1].get<float>(),
                value[2].get<float>()
            );

            return true;
        }

        glm::vec3 ReadColor(const json& value)
        {
            if (!value.is_array() || value.size() < 3)
                return glm::vec3(1.0f);

            float r = value[0].get<float>();
            float g = value[1].get<float>();
            float b = value[2].get<float>();

            // Blender may export color channels as bytes (0..255).
            if (r > 1.0f || g > 1.0f || b > 1.0f)
            {
                r /= 255.0f;
                g /= 255.0f;
                b /= 255.0f;
            }

            return glm::vec3(r, g, b);
        }

        glm::vec2 ReadVec2(const json& value)
        {
            if (!value.is_array() || value.size() < 2)
                return glm::vec2(0.0f);

            return glm::vec2(
                value[0].get<float>(),
                value[1].get<float>()
            );
        }

        glm::ivec4 ReadIVec4(const json& value)
        {
            if (!value.is_array() || value.size() < 4)
                return glm::ivec4(0);

            return glm::ivec4(
                value[0].get<int>(),
                value[1].get<int>(),
                value[2].get<int>(),
                value[3].get<int>()
            );
        }

        glm::vec4 ReadVec4(const json& value)
        {
            if (!value.is_array() || value.size() < 4)
                return glm::vec4(0.0f);

            return glm::vec4(
                value[0].get<float>(),
                value[1].get<float>(),
                value[2].get<float>(),
                value[3].get<float>()
            );
        }

        // ========================================================
        // Base64 decoder
        // ========================================================

        std::vector<unsigned char> DecodeBase64(
            const std::string& input
        )
        {
            static const signed char table[256] = {
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
                52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
                -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
                15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
                -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
                41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
            };

            std::vector<unsigned char> output;
            output.reserve((input.size() * 3) / 4);

            unsigned int accumulator = 0;
            int bits = -8;

            for (unsigned char c : input)
            {
                if (c == '=')
                    break;

                const signed char decoded = table[c];

                // Ignore whitespace and other non-Base64 characters.
                if (decoded < 0)
                    continue;

                accumulator =
                    (accumulator << 6) |
                    static_cast<unsigned int>(decoded);

                bits += 6;

                if (bits >= 0)
                {
                    output.push_back(
                        static_cast<unsigned char>(
                            (accumulator >> bits) & 0xFF
                        )
                    );

                    bits -= 8;
                }
            }

            return output;
        }

        // ========================================================
        // Animation transform helper
        // ========================================================

        glm::mat4 ComposeTRS(
            const glm::vec3& location,
            const glm::quat& rotation,
            const glm::vec3& scale
        )
        {
            return
                glm::translate(glm::mat4(1.0f), location)
                *
                glm::mat4_cast(rotation)
                *
                glm::scale(glm::mat4(1.0f), scale);
        }
    }

    // ============================================================
    // Constructors
    // ============================================================

    Model::Model(
        const char* file,
        unsigned int instancing,
        std::vector<glm::mat4> instanceMatrix
    )
        : instancing(instancing),
          instanceMatrix(std::move(instanceMatrix))
    {
        if (file == nullptr)
        {
            throw std::invalid_argument(
                "TimWorks Model: File path is null."
            );
        }

        std::ifstream input(file, std::ios::binary);

        if (!input)
        {
            throw std::runtime_error(
                std::string("TimWorks Model: Could not open file: ")
                + file
            );
        }

        const std::string jsonText(
            (std::istreambuf_iterator<char>(input)),
            std::istreambuf_iterator<char>()
        );

        sourceKey = std::string("file:") + file;

        loadFromJSONText(jsonText);
    }

    Model::Model(
        const std::vector<unsigned char>& bytes,
        unsigned int instancing,
        std::vector<glm::mat4> instanceMatrix
    )
        : instancing(instancing),
          instanceMatrix(std::move(instanceMatrix))
    {
        if (bytes.empty())
        {
            throw std::invalid_argument(
                "TimWorks Model: Byte data is empty."
            );
        }

        const std::string jsonText(
            bytes.begin(),
            bytes.end()
        );

        sourceKey = std::string("data:") + jsonText;

        loadFromJSONText(jsonText);
    }

    Model::Model(
        const std::string& data,
        ModelDataEncoding encoding,
        unsigned int instancing,
        std::vector<glm::mat4> instanceMatrix
    )
        : instancing(instancing),
          instanceMatrix(std::move(instanceMatrix))
    {
        std::string jsonText;

        if (encoding == ModelDataEncoding::JSON)
        {
            jsonText = data;
        }
        else
        {
            const std::vector<unsigned char> decoded =
                DecodeBase64(data);

            if (decoded.empty())
            {
                throw std::runtime_error(
                    "TimWorks Model: Base64 decoding produced no data."
                );
            }

            jsonText.assign(
                decoded.begin(),
                decoded.end()
            );
        }

        if (jsonText.empty())
        {
            throw std::invalid_argument(
                "TimWorks Model: JSON data is empty."
            );
        }

        sourceKey = std::string("data:") + jsonText;

        loadFromJSONText(jsonText);
    }

    // ============================================================
    // JSON loading pipeline
    // ============================================================

    void Model::loadFromJSONText(
        const std::string& jsonText
    )
    {
        try
        {
            JSON = json::parse(jsonText);
        }
        catch (const json::exception& e)
        {
            throw std::runtime_error(
                std::string("TimWorks Model: Invalid JSON: ")
                + e.what()
            );
        }

        const float version = JSON.value("version", 0.0f);

        if (version < 0.1f || version > 0.2f)
        {
            throw std::runtime_error(
                "TimWorks Model: Unsupported .tw_mdl version."
            );
        }

        if (!JSON.contains("idleFrame") ||
            !JSON["idleFrame"].is_object())
        {
            throw std::runtime_error(
                "TimWorks Model: Missing idleFrame object."
            );
        }

        loadMesh();
        loadSkeleton();
        loadAnimations();
    }

    // ============================================================
    // Destructor
    // ============================================================

    Model::~Model()
    {
        // Texture objects may be copied into several surface meshes.
        // Delete each OpenGL texture ID only once per model.
        std::unordered_set<unsigned int> deletedTextures;

        for (Mesh& mesh : meshes)
        {
            for (Material& material : mesh.materials)
            {
                for (Texture& texture : material.textures)
                {
                    if (texture.ID != 0 &&
                        deletedTextures.insert(texture.ID).second)
                    {
                        texture.Delete();
                    }

                    texture.ID = 0;
                }
            }
        }
    }

    // ============================================================
    // Matrix conversion
    // ============================================================

    glm::mat4 Model::readMatrix(
        const json& value
    ) const
    {
        if (!value.is_array() || value.size() != 16)
        {
            throw std::runtime_error(
                "TimWorks Model: Expected a 16-element matrix."
            );
        }

        glm::mat4 result(1.0f);

        // The Blender exporter serializes row by row.
        // GLM indexes matrices as [column][row].
        for (int row = 0; row < 4; ++row)
        {
            for (int column = 0; column < 4; ++column)
            {
                result[column][row] =
                    value[row * 4 + column].get<float>();
            }
        }

        return result;
    }

    // ============================================================
    // Mesh, vertex, surface, and material loading
    // ============================================================

    void Model::loadMesh()
    {
        const json& idle = JSON["idleFrame"];

        if (!idle.contains("vertices") ||
            !idle["vertices"].is_array())
        {
            throw std::runtime_error(
                "TimWorks Model: idleFrame.vertices is missing."
            );
        }

        if (!idle.contains("indices") ||
            !idle["indices"].is_array())
        {
            throw std::runtime_error(
                "TimWorks Model: idleFrame.indices is missing."
            );
        }

        // --------------------------------------------------------
        // Vertices
        // --------------------------------------------------------

        std::vector<Vertex> vertices;
        const json& jsonVertices = idle["vertices"];

        vertices.reserve(jsonVertices.size());

        for (const json& source : jsonVertices)
        {
            Vertex vertex{};

            if (!source.contains("position") ||
                !ReadVec3(source["position"], vertex.position))
            {
                throw std::runtime_error(
                    "TimWorks Model: Invalid vertex position."
                );
            }

            if (source.contains("normal"))
                ReadVec3(source["normal"], vertex.normal);

            if (source.contains("color"))
                vertex.color = ReadColor(source["color"]);
            else
                vertex.color = glm::vec3(1.0f);

            if (source.contains("texcoord"))
                vertex.texUV = ReadVec2(source["texcoord"]);

            if (source.contains("boneIds"))
                vertex.boneIDs = ReadIVec4(source["boneIds"]);

            if (source.contains("weights"))
                vertex.weights = ReadVec4(source["weights"]);

            vertices.push_back(vertex);
        }

        // --------------------------------------------------------
        // Indices
        // --------------------------------------------------------

        std::vector<unsigned int> indices;
        const json& jsonIndices = idle["indices"];

        indices.reserve(jsonIndices.size());

        for (const json& value : jsonIndices)
        {
            const int index = value.get<int>();

            if (index < 0 ||
                static_cast<std::size_t>(index) >= vertices.size())
            {
                throw std::runtime_error(
                    "TimWorks Model: Index references an invalid vertex."
                );
            }

            indices.push_back(
                static_cast<unsigned int>(index)
            );
        }

        // --------------------------------------------------------
        // Materials and embedded PNG textures
        // --------------------------------------------------------

        std::vector<Material> materials;

        if (JSON.contains("materials") &&
            JSON["materials"].is_array())
        {
            const json& jsonMaterials = JSON["materials"];

            materials.reserve(jsonMaterials.size());

            for (const json& sourceMaterial : jsonMaterials)
            {
                Material material;

                if (sourceMaterial.contains("texture") &&
                    sourceMaterial["texture"].is_object())
                {
                    const json& texture = sourceMaterial["texture"];

                    const bool embedded =
                        texture.value("embedded", false);

                    const std::string mime =
                        texture.value("mime", std::string());

                    const std::string encoded =
                        texture.value("data", std::string());

                    if (embedded && !encoded.empty())
                    {
                        if (mime != "image/png")
                        {
                            throw std::runtime_error(
                                "TimWorks Model: Unsupported texture MIME type: "
                                + mime
                            );
                        }

                        const std::vector<unsigned char> bytes =
                            DecodeBase64(encoded);

                        if (bytes.empty())
                        {
                            throw std::runtime_error(
                                "TimWorks Model: Embedded texture decoding failed."
                            );
                        }

                        if (bytes.size() >
                            static_cast<std::size_t>(
                                std::numeric_limits<int>::max()
                            ))
                        {
                            throw std::runtime_error(
                                "TimWorks Model: Embedded texture is too large."
                            );
                        }

                        Texture loadedTexture(
                            bytes.data(),
                            static_cast<int>(bytes.size()),
                            "diffuse",
                            GL_TEXTURE0
                        );

                        if (loadedTexture.ID == 0)
                        {
                            throw std::runtime_error(
                                "TimWorks Model: OpenGL texture creation failed."
                            );
                        }

                        material.textures.push_back(
                            std::move(loadedTexture)
                        );
                    }
                }

                materials.push_back(
                    std::move(material)
                );
            }
        }

        // --------------------------------------------------------
        // Surfaces
        //
        // Mesh::Draw draws the entire index buffer, so create a
        // separate mesh for each surface/material combination.
        // --------------------------------------------------------

        if (!idle.contains("surfaces") ||
            !idle["surfaces"].is_array() ||
            idle["surfaces"].empty())
        {
            std::vector<Material> meshMaterials;

            if (!materials.empty())
                meshMaterials.push_back(materials[0]);

            meshes.emplace_back(
                vertices,
                indices,
                std::move(meshMaterials),
                instancing,
                instanceMatrix
            );

            return;
        }

        const json& surfaces = idle["surfaces"];

        for (const json& surface : surfaces)
        {
            const std::size_t first =
                surface.value("firstIndex", std::size_t(0));

            const std::size_t count =
                surface.value("indexCount", std::size_t(0));

            if (first > indices.size() ||
                count > indices.size() - first)
            {
                throw std::runtime_error(
                    "TimWorks Model: Surface index range is invalid."
                );
            }

            if (count == 0)
                continue;

            std::vector<unsigned int> surfaceIndices(
                indices.begin() + first,
                indices.begin() + first + count
            );

            const int materialIndex =
                surface.value("material", -1);

            std::vector<Material> surfaceMaterials;

            if (materialIndex >= 0 &&
                static_cast<std::size_t>(materialIndex) <
                    materials.size())
            {
                surfaceMaterials.push_back(
                    materials[static_cast<std::size_t>(materialIndex)]
                );
            }

            meshes.emplace_back(
                vertices,
                std::move(surfaceIndices),
                std::move(surfaceMaterials),
                instancing,
                instanceMatrix
            );
        }
    }

    // ============================================================
    // Skeleton loading
    // ============================================================

    void Model::loadSkeleton()
    {
        bones.clear();

        const json& idle = JSON["idleFrame"];

        if (!idle.contains("bones") ||
            !idle["bones"].is_array())
        {
            return;
        }

        for (const json& source : idle["bones"])
        {
            ModelBone bone;

            bone.name = source.value(
                "name",
                std::string()
            );

            bone.parent = source.value("parent", -1);

            if (source.contains("transform"))
            {
                bone.transform = readMatrix(
                    source["transform"]
                );
            }

            bones.push_back(
                std::move(bone)
            );
        }
    }

    // ============================================================
    // Animation metadata loading
    // ============================================================

    void Model::loadAnimations()
    {
        frames.clear();
        animations.clear();

        // --------------------------------------------------------
        // Frames
        // --------------------------------------------------------

        if (JSON.contains("frames") &&
            JSON["frames"].is_array())
        {
            for (const json& sourceFrame : JSON["frames"])
            {
                ModelFrame frame;

                if (sourceFrame.contains("bones") &&
                    sourceFrame["bones"].is_array())
                {
                    for (const json& sourceBone :
                         sourceFrame["bones"])
                    {
                        ModelBonePose pose;

                        pose.name = sourceBone.value(
                            "name",
                            std::string()
                        );

                        pose.parent = sourceBone.value(
                            "parent",
                            -1
                        );

                        if (sourceBone.contains("location") &&
                            sourceBone.contains("rotation") &&
                            sourceBone.contains("scale"))
                        {
                            if (!ReadVec3(
                                sourceBone["location"],
                                pose.location))
                            {
                                throw std::runtime_error(
                                    "TimWorks Model: Invalid bone location."
                                );
                            }

                            const json& q =
                                sourceBone["rotation"];

                            if (!q.is_array() || q.size() < 4)
                            {
                                throw std::runtime_error(
                                    "TimWorks Model: Invalid bone quaternion."
                                );
                            }

                            // Exporter quaternion order is W, X, Y, Z.
                            pose.rotation = glm::quat(
                                q[0].get<float>(),
                                q[1].get<float>(),
                                q[2].get<float>(),
                                q[3].get<float>()
                            );

                            if (!ReadVec3(
                                sourceBone["scale"],
                                pose.scale))
                            {
                                throw std::runtime_error(
                                    "TimWorks Model: Invalid bone scale."
                                );
                            }

                            pose.usesTRS = true;

                            pose.transform = ComposeTRS(
                                pose.location,
                                pose.rotation,
                                pose.scale
                            );
                        }
                        else if (sourceBone.contains("transform"))
                        {
                            pose.transform = readMatrix(
                                sourceBone["transform"]
                            );

                            pose.usesTRS = false;
                        }

                        frame.bones.push_back(
                            std::move(pose)
                        );
                    }
                }

                frames.push_back(
                    std::move(frame)
                );
            }
        }

        // --------------------------------------------------------
        // Animation clips and key references
        // --------------------------------------------------------

        if (JSON.contains("animations") &&
            JSON["animations"].is_array())
        {
            for (const json& sourceAnimation : JSON["animations"])
            {
                ModelAnimation animation;

                animation.name = sourceAnimation.value(
                    "name",
                    std::string()
                );

                if (sourceAnimation.contains("keys") &&
                    sourceAnimation["keys"].is_array())
                {
                    for (const json& sourceKey :
                         sourceAnimation["keys"])
                    {
                        AnimationKey key;

                        key.frame = sourceKey.value(
                            "frame",
                            0u
                        );

                        key.time = sourceKey.value(
                            "time",
                            0.0f
                        );

                        key.lerp = sourceKey.value(
                            "lerp",
                            true
                        );

                        if (key.frame >= frames.size())
                        {
                            throw std::runtime_error(
                                "TimWorks Model: Animation key references "
                                "a nonexistent frame."
                            );
                        }

                        animation.keys.push_back(key);
                    }
                }

                animations.push_back(
                    std::move(animation)
                );
            }
        }
    }

    // ============================================================
    // Drawing
    // ============================================================

    void Model::Draw(
        Camera& camera,
        const glm::mat4& transform
    )
    {
        for (Mesh& mesh : meshes)
        {
            mesh.Draw(
                camera,
                transform
            );
        }
    }

    // ============================================================
    // Accessors
    // ============================================================

    const std::vector<ModelBone>& Model::GetBones() const
    {
        return bones;
    }

    const std::vector<ModelFrame>& Model::GetFrames() const
    {
        return frames;
    }

    const std::vector<ModelAnimation>& Model::GetAnimations() const
    {
        return animations;
    }

    // ============================================================
    // Cache initialization and shutdown
    // ============================================================

    bool Model::Init()
    {
        return true;
    }

    void Model::Shutdown()
    {
        for (Model* model : modelCache)
            delete model;

        modelCache.clear();
        modelCacheKeys.clear();
    }

    // ============================================================
    // Cache: file
    // ============================================================

    ModelHandle Model::Pre_CacheModel(
        const std::string& path
    )
    {
        const std::string key = std::string("file:") + path;

        for (std::size_t i = 0; i < modelCacheKeys.size(); ++i)
        {
            if (modelCacheKeys[i] == key)
                return static_cast<ModelHandle>(i);
        }

        Model* model = new Model(path.c_str());

        modelCache.push_back(model);
        modelCacheKeys.push_back(key);

        return static_cast<ModelHandle>(
            modelCache.size() - 1
        );
    }

    // ============================================================
    // Cache: raw JSON bytes
    // ============================================================

    ModelHandle Model::Pre_CacheModel(
        const std::vector<unsigned char>& bytes
    )
    {
        if (bytes.empty())
        {
            throw std::invalid_argument(
                "TimWorks Model: Cannot cache empty byte data."
            );
        }

        const std::string jsonText(
            bytes.begin(),
            bytes.end()
        );

        const std::string key = std::string("data:") + jsonText;

        for (std::size_t i = 0; i < modelCacheKeys.size(); ++i)
        {
            if (modelCacheKeys[i] == key)
                return static_cast<ModelHandle>(i);
        }

        Model* model = new Model(bytes);

        modelCache.push_back(model);
        modelCacheKeys.push_back(key);

        return static_cast<ModelHandle>(
            modelCache.size() - 1
        );
    }

    // ============================================================
    // Cache: Base64-encoded complete model
    // ============================================================

    ModelHandle Model::Pre_CacheModelBase64(
        const std::string& base64
    )
    {
        const std::vector<unsigned char> decoded =
            DecodeBase64(base64);

        if (decoded.empty())
        {
            throw std::runtime_error(
                "TimWorks Model: Base64 model data is invalid or empty."
            );
        }

        // The byte overload deduplicates using decoded JSON content.
        return Pre_CacheModel(decoded);
    }

    // ============================================================
    // Cache: JSON text
    // ============================================================

    ModelHandle Model::Pre_CacheModelJSON(
        const std::string& jsonText
    )
    {
        if (jsonText.empty())
        {
            throw std::invalid_argument(
                "TimWorks Model: Cannot cache empty JSON text."
            );
        }

        const std::vector<unsigned char> bytes(
            jsonText.begin(),
            jsonText.end()
        );

        // Reuse the byte cache so JSON and Base64 versions
        // of the same model can share the same cache entry.
        return Pre_CacheModel(bytes);
    }

    // ============================================================
    // Draw cached model
    // ============================================================

    void Model::DrawModel(
        ModelHandle model,
        Camera& camera,
        const glm::mat4& transform
    )
    {
        if (static_cast<std::size_t>(model) >= modelCache.size())
        {
            std::cerr
                << "[TimWorks] Invalid model handle: "
                << model
                << '\n';

            return;
        }

        Model* cachedModel = modelCache[model];

        if (cachedModel == nullptr)
            return;

        cachedModel->Draw(camera, transform);
    }
}