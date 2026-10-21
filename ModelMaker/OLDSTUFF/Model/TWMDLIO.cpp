#include "TWMDLIO.h"

#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace TW
{

static json Vector2ToJson(
    const Vector2& v
)
{
    return json{
        v.x,
        v.y
    };
}


static Vector2 JsonToVector2(
    const json& j
)
{
    return {
        j[0].get<float>(),
        j[1].get<float>()
    };
}


static json Vector3ToJson(
    const Vector3& v
)
{
    return json{
        v.x,
        v.y,
        v.z
    };
}


static Vector3 JsonToVector3(
    const json& j
)
{
    return {
        j[0].get<float>(),
        j[1].get<float>(),
        j[2].get<float>()
    };
}


static json ColorToJson(
    const Color& c
)
{
    return json{
        c.r,
        c.g,
        c.b,
        c.a
    };
}


static Color JsonToColor(
    const json& j
)
{
    return Color{
        j[0].get<unsigned char>(),
        j[1].get<unsigned char>(),
        j[2].get<unsigned char>(),
        j[3].get<unsigned char>()
    };
}


static json VertexToJson(
    const ModelVertex& vertex
)
{
    json j;

    j["position"] =
        Vector3ToJson(vertex.position);

    j["normal"] =
        Vector3ToJson(vertex.normal);

    j["color"] =
        ColorToJson(vertex.color);

    j["texcoord"] =
        Vector2ToJson(vertex.texcoord);

    j["bone_ids"] = {
        vertex.boneIds[0],
        vertex.boneIds[1],
        vertex.boneIds[2],
        vertex.boneIds[3]
    };

    j["weights"] = {
        vertex.weights[0],
        vertex.weights[1],
        vertex.weights[2],
        vertex.weights[3]
    };

    return j;
}


static ModelVertex JsonToVertex(
    const json& j
)
{
    ModelVertex vertex;

    vertex.position =
        JsonToVector3(j["position"]);

    vertex.normal =
        JsonToVector3(j["normal"]);

    vertex.color =
        JsonToColor(j["color"]);

    vertex.texcoord =
        JsonToVector2(j["texcoord"]);

    for (int i = 0; i < 4; i++)
    {
        vertex.boneIds[i] =
            j["bone_ids"][i].get<int>();

        vertex.weights[i] =
            j["weights"][i].get<float>();
    }

    return vertex;
}


static json MatrixToJson(
    const Matrix& m
)
{
    return json{
        m.m0,  m.m1,  m.m2,  m.m3,
        m.m4,  m.m5,  m.m6,  m.m7,
        m.m8,  m.m9,  m.m10, m.m11,
        m.m12, m.m13, m.m14, m.m15
    };
}


static Matrix JsonToMatrix(
    const json& j
)
{
    Matrix m;

    m.m0  = j[0].get<float>();
    m.m1  = j[1].get<float>();
    m.m2  = j[2].get<float>();
    m.m3  = j[3].get<float>();

    m.m4  = j[4].get<float>();
    m.m5  = j[5].get<float>();
    m.m6  = j[6].get<float>();
    m.m7  = j[7].get<float>();

    m.m8  = j[8].get<float>();
    m.m9  = j[9].get<float>();
    m.m10 = j[10].get<float>();
    m.m11 = j[11].get<float>();

    m.m12 = j[12].get<float>();
    m.m13 = j[13].get<float>();
    m.m14 = j[14].get<float>();
    m.m15 = j[15].get<float>();

    return m;
}


static json FrameToJson(
    const ModelFrame& frame
)
{
    json j;

    j["vertices"] =
        json::array();

    for (const auto& vertex : frame.vertices)
        j["vertices"].push_back(
            VertexToJson(vertex)
        );


    j["indices"] =
        frame.indices;


    j["bones"] =
        json::array();

    for (const auto& bone : frame.bones)
    {
        j["bones"].push_back({
            {
                "name",
                bone.name
            },
            {
                "parent",
                bone.parent
            },
            {
                "transform",
                MatrixToJson(bone.transform)
            }
        });
    }


    j["surfaces"] =
        json::array();

    for (const auto& surface : frame.surfaces)
    {
        j["surfaces"].push_back({
            {
                "first_index",
                surface.firstIndex
            },
            {
                "index_count",
                surface.indexCount
            },
            {
                "material",
                surface.material
            },
            {
                "shading",
                surface.shading ==
                    SurfaceShading::Flat
                    ? "flat"
                    : "smooth"
            }
        });
    }

    return j;
}


static ModelFrame JsonToFrame(
    const json& j
)
{
    ModelFrame frame;


    for (const auto& vertex :
         j["vertices"])
    {
        frame.vertices.push_back(
            JsonToVertex(vertex)
        );
    }


    frame.indices =
        j["indices"]
        .get<std::vector<unsigned int>>();


    for (const auto& bone :
         j["bones"])
    {
        ModelBone b;

        b.name =
            bone["name"]
            .get<std::string>();

        b.parent =
            bone["parent"]
            .get<int>();

        b.transform =
            JsonToMatrix(
                bone["transform"]
            );

        frame.bones.push_back(
            b
        );
    }


    for (const auto& surface :
         j["surfaces"])
    {
        ModelSurface s;

        s.firstIndex =
            surface["first_index"]
            .get<unsigned int>();

        s.indexCount =
            surface["index_count"]
            .get<unsigned int>();

        s.material =
            surface["material"]
            .get<int>();

        std::string shading =
            surface["shading"]
            .get<std::string>();

        s.shading =
            shading == "flat"
                ? SurfaceShading::Flat
                : SurfaceShading::Smooth;

        frame.surfaces.push_back(
            s
        );
    }

    return frame;
}


bool SaveTWModel(
    const TWModel& model,
    const std::string& filename
)
{
    json root;

    root["version"] =
        model.version;


    root["materials"] =
        json::array();

    for (const auto& material :
         model.materials)
    {
        root["materials"].push_back({
            {
                "name",
                material.name
            },
            {
                "texture",
                {
                    {
                        "embedded",
                        material.texture.embedded
                    },
                    {
                        "mime",
                        material.texture.mime
                    },
                    {
                        "data",
                        material.texture.data
                    }
                }
            }
        });
    }


    root["idle_frame"] =
        FrameToJson(
            model.idleFrame
        );


    root["collision_data"] = {
        {
            "vertices",
            json::array()
        },
        {
            "indices",
            model.collision.indices
        },
        {
            "surfaces",
            json::array()
        }
    };


    for (const auto& vertex :
         model.collision.vertices)
    {
        root["collision_data"]
            ["vertices"]
            .push_back(
                Vector3ToJson(vertex)
            );
    }


    for (const auto& surface :
         model.collision.surfaces)
    {
        root["collision_data"]
            ["surfaces"]
            .push_back({
                {
                    "first_index",
                    surface.firstIndex
                },
                {
                    "index_count",
                    surface.indexCount
                }
            });
    }


    root["frames"] =
        json::array();

    for (const auto& frame :
         model.frames)
    {
        root["frames"].push_back(
            FrameToJson(frame)
        );
    }


    root["animations"] =
        json::object();

    for (const auto& animation :
         model.animations)
    {
        json keys = json::array();

        for (const auto& key :
             animation.keys)
        {
            keys.push_back({
                {
                    "frame",
                    key.frame
                },
                {
                    "time",
                    key.time
                },
                {
                    "lerp",
                    key.lerp
                }
            });
        }

        root["animations"]
            [animation.name] = keys;
    }


    std::ofstream file(
        filename
    );

    if (!file)
    {
        std::cerr
            << "Could not save TW_MDL: "
            << filename
            << '\n';

        return false;
    }


    file << root.dump(4);

    return true;
}


bool LoadTWModel(
    TWModel& model,
    const std::string& filename
)
{
    std::ifstream file(
        filename
    );

    if (!file)
    {
        std::cerr
            << "Could not open TW_MDL: "
            << filename
            << '\n';

        return false;
    }


    try
    {
        json root;

        file >> root;


        model =
            TWModel();


        model.version =
            root["version"]
            .get<float>();


        for (const auto& material :
             root["materials"])
        {
            ModelMaterial m;

            m.name =
                material["name"]
                .get<std::string>();

            const auto& texture =
                material["texture"];

            m.texture.embedded =
                texture["embedded"]
                .get<bool>();

            m.texture.mime =
                texture["mime"]
                .get<std::string>();

            m.texture.data =
                texture["data"]
                .get<std::string>();

            model.materials.push_back(
                m
            );
        }


        model.idleFrame =
            JsonToFrame(
                root["idle_frame"]
            );


        const auto& collision =
            root["collision_data"];


        for (const auto& vertex :
             collision["vertices"])
        {
            model.collision.vertices.push_back(
                JsonToVector3(vertex)
            );
        }


        model.collision.indices =
            collision["indices"]
            .get<std::vector<unsigned int>>();


        for (const auto& surface :
             collision["surfaces"])
        {
            CollisionSurface s;

            s.firstIndex =
                surface["first_index"]
                .get<unsigned int>();

            s.indexCount =
                surface["index_count"]
                .get<unsigned int>();

            model.collision.surfaces.push_back(
                s
            );
        }


        for (const auto& frame :
             root["frames"])
        {
            model.frames.push_back(
                JsonToFrame(frame)
            );
        }


        for (auto& [name, keys] :
             root["animations"].items())
        {
            ModelAnimation animation;

            animation.name = name;

            for (const auto& key :
                 keys)
            {
                AnimationKey k;

                k.frame =
                    key["frame"]
                    .get<int>();

                k.time =
                    key["time"]
                    .get<float>();

                k.lerp =
                    key["lerp"]
                    .get<bool>();

                animation.keys.push_back(
                    k
                );
            }

            model.animations.push_back(
                animation
            );
        }

    }
    catch (const std::exception& e)
    {
        std::cerr
            << "Invalid TW_MDL: "
            << e.what()
            << '\n';

        return false;
    }

    return true;
}

}