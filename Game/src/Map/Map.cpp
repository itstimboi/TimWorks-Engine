#include "Map.h"

#include <fstream>
#include <iostream>

#include <json/json.hpp>

using json = nlohmann::json;

namespace GAME
{

    std::vector<Map*> Map::mapCache = {};

    // ------------------------------------------------------------
    // MapVertex -> JSON
    // ------------------------------------------------------------

    static json VertexToJSON(
        const MapVertex& vertex
    )
    {
        return
        {
            {
                "position",
                {
                    vertex.position.x,
                    vertex.position.y,
                    vertex.position.z
                }
            },

            {
                "normal",
                {
                    vertex.normal.x,
                    vertex.normal.y,
                    vertex.normal.z
                }
            },

            {
                "color",
                {
                    vertex.color.x,
                    vertex.color.y,
                    vertex.color.z
                }
            },

            {
                "uv",
                {
                    vertex.uv.x,
                    vertex.uv.y
                }
            }
        };
    }

    bool Map::Init()
    {
        mapCache.clear();

        return true;
    }

    void Map::Shutdown()
    {
        for (Map* Map : mapCache)
        {
            delete Map;
        }

        mapCache.clear();
    }

    Map::Map(const char* file)
    {
        Map::file = file;

        if (!Load(file))
        {
            throw std::runtime_error(
                std::string("Failed to load map: ") + file
            );
        }
    }

    MapHandle Map::Pre_CacheMap(
        const std::string& path
    )
    {
        Map* map =
            new Map(
                path.c_str()
            );

        mapCache.push_back(
            map
        );

        return static_cast<MapHandle>(
            mapCache.size() - 1
        );
    }

    // ------------------------------------------------------------
    // JSON -> MapVertex
    // ------------------------------------------------------------

    static MapVertex VertexFromJSON(
        const json& jsonVertex
    )
    {
        MapVertex vertex;

        const auto& position = jsonVertex["position"];

        vertex.position = glm::vec3(
            position[0],
            position[1],
            position[2]
        );


        const auto& normal = jsonVertex["normal"];

        vertex.normal = glm::vec3(
            normal[0],
            normal[1],
            normal[2]
        );


        const auto& color = jsonVertex["color"];

        vertex.color = glm::vec3(
            color[0],
            color[1],
            color[2]
        );


        const auto& uv = jsonVertex["uv"];

        vertex.uv = glm::vec2(
            uv[0],
            uv[1]
        );


        return vertex;
    }


    // ------------------------------------------------------------
    // MapEntity -> JSON
    // ------------------------------------------------------------

    static json EntityToJSON(
        const MapEntity& entity
    )
    {
        return
        {
            {
                "type",
                entity.type
            },

            {
                "position",
                {
                    entity.position.x,
                    entity.position.y,
                    entity.position.z
                }
            },

            {
                "rotation",
                {
                    entity.rotation.x,
                    entity.rotation.y,
                    entity.rotation.z
                }
            },

            {
                "scale",
                {
                    entity.scale.x,
                    entity.scale.y,
                    entity.scale.z
                }
            }
        };
    }


    // ------------------------------------------------------------
    // JSON -> MapEntity
    // ------------------------------------------------------------

    static MapEntity EntityFromJSON(
        const json& jsonEntity
    )
    {
        MapEntity entity;

        entity.type = jsonEntity["type"];


        const auto& position =
            jsonEntity["position"];

        entity.position = glm::vec3(
            position[0],
            position[1],
            position[2]
        );


        const auto& rotation =
            jsonEntity["rotation"];

        entity.rotation = glm::vec3(
            rotation[0],
            rotation[1],
            rotation[2]
        );


        const auto& scale =
            jsonEntity["scale"];

        entity.scale = glm::vec3(
            scale[0],
            scale[1],
            scale[2]
        );


        return entity;
    }

    static MapMaterial MaterialFromJSON(
        const json& jsonMaterial
    )
    {
        MapMaterial material;

        material.name =
            jsonMaterial.value(
                "name",
                "Unnamed Material"
            );

        material.texture =
            jsonMaterial.value(
                "texture",
                ""
            );

        return material;
    }

    static MapSurface SurfaceFromJSON(
        const json& jsonSurface
    )
    {
        MapSurface surface;

        surface.material =
            jsonSurface.value(
                "material",
                0
            );

        // --------------------------------------------------------
        // Edges
        // --------------------------------------------------------

        if (jsonSurface.contains("edges"))
        {
            for (const auto& edge : jsonSurface["edges"])
            {
                surface.edges.push_back(
                    edge.get<unsigned int>()
                );
            }
        }

        return surface;
    }

    static MapEdge EdgeFromJSON(
        const json& jsonEdge
    )
    {
        MapEdge edge;

        edge.vertexA =
            jsonEdge.value(
                "vertexA",
                0u
            );

        edge.vertexB =
            jsonEdge.value(
                "vertexB",
                0u
            );

        return edge;
    }

    static json EdgeToJSON(
        const MapEdge& edge
    )
    {
        return
        {
            {
                "vertexA",
                edge.vertexA
            },

            {
                "vertexB",
                edge.vertexB
            }
        };
    }

    // ------------------------------------------------------------
    // New
    // ------------------------------------------------------------

    void Map::New(
        const std::string& name
    )
    {
        data = MapData{};

        data.version = 1;
        data.name = name;
    }


    // ------------------------------------------------------------
    // Save
    // ------------------------------------------------------------

    // bool Map::Save(
    //     const std::string& filename
    // ) const
    // {
    //     json map;


    //     // --------------------------------------------------------
    //     // Basic information
    //     // --------------------------------------------------------

    //     map["version"] = data.version;
    //     map["name"] = data.name;


    //     // --------------------------------------------------------
    //     // Vertices
    //     // --------------------------------------------------------

    //     map["vertices"] = json::array();

    //     for (const MapVertex& vertex : data.vertices)
    //     {
    //         map["vertices"].push_back(
    //             VertexToJSON(vertex)
    //         );
    //     }


    //     // --------------------------------------------------------
    //     // Indices
    //     // --------------------------------------------------------

    //     map["indices"] = data.indices;


    //     // --------------------------------------------------------
    //     // Entities
    //     // --------------------------------------------------------

    //     map["entities"] = json::array();

    //     for (const MapEntity& entity : data.entities)
    //     {
    //         map["entities"].push_back(
    //             EntityToJSON(entity)
    //         );
    //     }


    //     // --------------------------------------------------------
    //     // Write file
    //     // --------------------------------------------------------

    //     std::ofstream file(
    //         filename
    //     );

    //     if (!file.is_open())
    //     {
    //         std::cerr
    //             << "Failed to save map: "
    //             << filename
    //             << '\n';

    //         return false;
    //     }


    //     // Pretty-print JSON with 4 spaces
    //     file << map.dump(4);

    //     file.close();


    //     std::cout
    //         << "Saved map: "
    //         << filename
    //         << '\n';


    //     return true;
    // }

    static TE::Vertex MapVertexToVertex(
        const MapVertex& mapVertex
    )
    {
        TE::Vertex vertex;

        vertex.position = mapVertex.position;
        vertex.normal = mapVertex.normal;
        vertex.color = mapVertex.color;
        vertex.texUV = mapVertex.uv;

        return vertex;
    }

    /*bool Map::BuildMeshes()
    {
        meshes.clear();

        if (data.vertices.empty())
        {
            std::cerr
                << "Map contains no vertices.\n";

            return false;
        }

        // if (data.indices.empty())
        // {
        //     std::cerr
        //         << "Map contains no indices.\n";

        //     return false;
        // }

        std::vector<Vertex> vertices;

        vertices.reserve(
            data.vertices.size()
        );

        for (const MapSurface& surface : data.surfaces)
        {
            std::vector<Vertex> vertices;

            for (const MapVertex& mapVertex : data.vertices)
            {
                Vertex vertex;

                vertex.position = mapVertex.position;
                vertex.normal = mapVertex.normal;
                vertex.color = mapVertex.color;
                vertex.texUV = mapVertex.uv;

                vertices.push_back(vertex);
            }

            std::vector<unsigned int> indices;

            for (unsigned int index : surface.indices)
            {
                indices.push_back(index);
            }

            std::vector<Texture> textures;

            // Load surface.material's texture here

            meshes.emplace_back(
                vertices,
                indices,
                textures
            );
        }

        std::vector<unsigned int> indices;

        // indices.reserve(
        //     data.indices.size()
        // );

        // for (unsigned int index : data.indices)
        // {
        //     indices.push_back(index);
        // }

        std::vector<Texture> textures;

        meshes.emplace_back(
            vertices,
            indices,
            textures
        );

        return true;
    }*/

    bool Map::BuildMeshes()
    {
        meshes.clear();

        // --------------------------------------------------------
        // Build one mesh per surface
        // --------------------------------------------------------

        for (const MapSurface& surface :
            data.surfaces)
        {
            // ----------------------------------------------------
            // Validate material
            // ----------------------------------------------------

            if (surface.material >=
                data.materials.size())
            {
                std::cerr
                    << "Map surface references invalid "
                    << "material: "
                    << surface.material
                    << '\n';

                return false;
            }


            // ----------------------------------------------------
            // Surface vertices
            //
            // surface
            //    ↓
            // edges
            //    ↓
            // vertices
            // ----------------------------------------------------

            std::vector<unsigned int> surfaceVertices;

            for (unsigned int edgeIndex :
                surface.edges)
            {
                // -----------------------------------------------
                // Validate edge
                // -----------------------------------------------

                if (edgeIndex >= data.edges.size())
                {
                    std::cerr
                        << "Surface references invalid edge: "
                        << edgeIndex
                        << '\n';

                    return false;
                }

                const MapEdge& edge =
                    data.edges[edgeIndex];


                // -----------------------------------------------
                // Validate vertex A
                // -----------------------------------------------

                if (edge.vertexA >= data.vertices.size())
                {
                    std::cerr
                        << "Edge "
                        << edgeIndex
                        << " references invalid vertex A: "
                        << edge.vertexA
                        << '\n';

                    return false;
                }


                // -----------------------------------------------
                // Validate vertex B
                // -----------------------------------------------

                if (edge.vertexB >= data.vertices.size())
                {
                    std::cerr
                        << "Edge "
                        << edgeIndex
                        << " references invalid vertex B: "
                        << edge.vertexB
                        << '\n';

                    return false;
                }


                // -----------------------------------------------
                // Add vertices
                // -----------------------------------------------

                if (surfaceVertices.empty())
                {
                    surfaceVertices.push_back(
                        edge.vertexA
                    );

                    surfaceVertices.push_back(
                        edge.vertexB
                    );
                }
                else
                {
                    // -------------------------------------------
                    // Add only the new endpoint.
                    //
                    // This assumes edges are stored in the
                    // correct order around the surface.
                    // -------------------------------------------

                    unsigned int lastVertex =
                        surfaceVertices.back();

                    if (edge.vertexA == lastVertex)
                    {
                        surfaceVertices.push_back(
                            edge.vertexB
                        );
                    }
                    else if (edge.vertexB == lastVertex)
                    {
                        surfaceVertices.push_back(
                            edge.vertexA
                        );
                    }
                    else
                    {
                        // ---------------------------------------
                        // Edge isn't connected to previous edge.
                        // ---------------------------------------

                        std::cerr
                            << "Surface contains disconnected "
                            << "edges.\n";

                        return false;
                    }
                }
            }


            // ----------------------------------------------------
            // Need at least one triangle
            // ----------------------------------------------------

            if (surfaceVertices.size() < 3)
            {
                std::cerr
                    << "Surface contains fewer than 3 vertices.\n";

                continue;
            }


            // ----------------------------------------------------
            // Remove duplicate closing vertex
            //
            // Example:
            //
            // 0 -> 1 -> 2 -> 3 -> 0
            //
            // becomes:
            //
            // 0 -> 1 -> 2 -> 3
            // ----------------------------------------------------

            if (surfaceVertices.front() ==
                surfaceVertices.back())
            {
                surfaceVertices.pop_back();
            }


            // ----------------------------------------------------
            // Create vertices
            // ----------------------------------------------------

            std::vector<TE::Vertex> vertices;

            vertices.reserve(
                surfaceVertices.size()
            );

            for (unsigned int vertexIndex :
                surfaceVertices)
            {
                if (vertexIndex >= data.vertices.size())
                {
                    std::cerr
                        << "Invalid surface vertex: "
                        << vertexIndex
                        << '\n';

                    return false;
                }

                const MapVertex& mapVertex =
                    data.vertices[vertexIndex];

                TE::Vertex vertex;

                vertex.position =
                    mapVertex.position;

                vertex.normal =
                    mapVertex.normal;

                vertex.color =
                    mapVertex.color;

                vertex.texUV =
                    mapVertex.uv;

                vertices.push_back(
                    vertex
                );
            }


            // ----------------------------------------------------
            // Triangulate polygon
            //
            // For:
            //
            // 0 1 2 3
            //
            // produces:
            //
            // 0 1 2
            // 0 2 3
            // ----------------------------------------------------

            std::vector<unsigned int> indices;

            for (size_t i = 1;
                i + 1 < surfaceVertices.size();
                i++)
            {
                indices.push_back(0);

                indices.push_back(
                    static_cast<unsigned int>(i)
                );

                indices.push_back(
                    static_cast<unsigned int>(i + 1)
                );
            }


            // ----------------------------------------------------
            // Load texture
            // ----------------------------------------------------

            std::vector<TE::Texture> textures;

            const MapMaterial& material =
                data.materials[surface.material];

            if (!material.texture.empty())
            {
                textures.emplace_back(
                    material.texture.c_str(),
                    "diffuse",
                    GL_TEXTURE0
                );
            }


            // ----------------------------------------------------
            // Create mesh
            // ----------------------------------------------------

            meshes.emplace_back(
				TE::Mesh(
					vertices,
					indices,
					textures
				)
            );
        }


        std::cout
            << "Built "
            << meshes.size()
            << " map meshes\n";


        return true;
    }

    // ------------------------------------------------------------
    // Load
    // ------------------------------------------------------------

    bool Map::Load(const std::string& filename)
    {
        std::ifstream file(
            filename
        );

        if (!file.is_open())
        {
            std::cerr
                << "Failed to open map: "
                << filename
                << '\n';

            return false;
        }


        json map;

        try
        {
            file >> map;
        }
        catch (const json::parse_error& error)
        {
            std::cerr
                << "Failed to parse map: "
                << error.what()
                << '\n';

            return false;
        }


        // --------------------------------------------------------
        // Version
        // --------------------------------------------------------

        if (!map.contains("version"))
        {
            std::cerr
                << "Map does not contain a version!\n";

            return false;
        }

        data.version = map["version"];


        // --------------------------------------------------------
        // Name
        // --------------------------------------------------------

        if (map.contains("name"))
        {
            data.name = map["name"];
        }
        else
        {
            data.name = "Untitled Map";
        }


        // --------------------------------------------------------
        // Clear old data
        // --------------------------------------------------------

        // data.vertices.clear();
        // data.indices.clear();
        // data.entities.clear();

        data.materials.clear();
        data.vertices.clear();
        data.edges.clear();
        data.surfaces.clear();
        data.entities.clear();

        // --------------------------------------------------------
        // Materials
        // --------------------------------------------------------


        if (map.contains("materials"))
        {
            for (const auto& jsonMaterial :
                map["materials"])
            {
                data.materials.push_back(
                    MaterialFromJSON(
                        jsonMaterial
                    )
                );
            }
        }

        // --------------------------------------------------------
        // Vertices
        // --------------------------------------------------------

        if (map.contains("vertices"))
        {
            for (const auto& jsonVertex : map["vertices"])
            {
                data.vertices.push_back(
                    VertexFromJSON(jsonVertex)
                );
            }
        }

        // --------------------------------------------------------
        // Edges
        // --------------------------------------------------------

        data.edges.clear();

        if (map.contains("edges"))
        {
            for (const auto& jsonEdge :
                map["edges"])
            {
                data.edges.push_back(
                    EdgeFromJSON(
                        jsonEdge
                    )
                );
            }
        }


        // --------------------------------------------------------
        // Indices
        // --------------------------------------------------------

        // if (map.contains("indices"))
        // {
        //     for (const auto& index : map["indices"])
        //     {
        //         data.indices.push_back(
        //             index.get<unsigned int>()
        //         );
        //     }
        // }

        // --------------------------------------------------------
        // Surfaces
        // --------------------------------------------------------

        if (map.contains("surfaces"))
        {
            for (const auto& jsonSurface :
                map["surfaces"])
            {
                data.surfaces.push_back(
                    SurfaceFromJSON(
                        jsonSurface
                    )
                );
            }
        }


        // --------------------------------------------------------
        // Entities
        // --------------------------------------------------------

        if (map.contains("entities"))
        {
            for (const auto& jsonEntity : map["entities"])
            {
                data.entities.push_back(
                    EntityFromJSON(jsonEntity)
                );
            }
        }

        std::cout
            << "Loaded map: "
            << filename
            << '\n';
            
        std::cout
            << "Materials: "
            << data.materials.size()
            << '\n';

        std::cout
            << "Vertices: "
            << data.vertices.size()
            << '\n';

        std::cout
            << "Edges: "
            << data.edges.size()
            << '\n';

        // std::cout
        //     << "Indices: "
        //     << data.indices.size()
        //     << '\n';

        

        std::cout
            << "Surfaces: "
            << data.surfaces.size()
            << '\n';

        std::cout
            << "Entities: "
            << data.entities.size()
            << '\n';

        if (!BuildMeshes())
        {
            std::cerr
                << "Failed to build meshes for map: "
                << filename
                << '\n';

            return false;
        }

        return true;
    }

    /*void Map::Draw(
        MapHandle map,
        Shader& shader,
        Camera& camera
    )
    {
        if (map >= mapCache.size())
            return;

        Map* currentMap = mapCache[map];

        for (Mesh& mesh : currentMap->meshes)
        {
            mesh.Draw(
                shader,
                camera,
                glm::mat4(1.0f)
            );
        }
    }*/

    void Map::Draw(
        MapHandle map,
        TE::Shader& shader,
        TE::Camera& camera
    )
    {
        if (map >= mapCache.size())
        {
            std::cerr
                << "Invalid MapHandle: "
                << map
                << '\n';

            return;
        }

        Map* currentMap =
            mapCache[map];

        for (TE::Mesh& mesh :
            currentMap->meshes)
        {
            mesh.Draw(
                shader,
                camera
            );
        }
    }

    // ------------------------------------------------------------
    // GetData
    // ------------------------------------------------------------

    MapData& Map::GetData()
    {
        return data;
    }


    const MapData& Map::GetData() const
    {
        return data;
    }
}