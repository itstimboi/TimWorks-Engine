#include <iostream>
#include <filesystem>
#include <cmath>

#include "raylib/raylib.h"
#include "raylib/raymath.h"

#include "ImGUI/imgui.h"
#include "rlImGUI/rlImGui.h"

#include "Map/Map.h"
#include "FileDialog.h"

void CreateDefaultMap(Map& map)
{
    EditorData& data = map.GetData();

    // --------------------------------------------------------
    // Clear old data
    // --------------------------------------------------------

    data.vertices.clear();
    data.edges.clear();
    data.surfaces.clear();
    data.entities.clear();
    data.materials.clear();


    // ========================================================
    // Vertices
    // ========================================================

    data.vertices =
    {
        // ----------------------------------------------------
        // Floor
        // ----------------------------------------------------

        EditorVertex{
            glm::vec3(-5.0f, 0.0f, -5.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(1.0f),
            glm::vec2(0.0f, 0.0f)
        },

        EditorVertex{
            glm::vec3(5.0f, 0.0f, -5.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(1.0f),
            glm::vec2(2.0f, 0.0f)
        },

        EditorVertex{
            glm::vec3(5.0f, 0.0f, 5.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(1.0f),
            glm::vec2(2.0f, 2.0f)
        },

        EditorVertex{
            glm::vec3(-5.0f, 0.0f, 5.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(1.0f),
            glm::vec2(0.0f, 2.0f)
        },


        // ----------------------------------------------------
        // Wall 1
        // ----------------------------------------------------

        EditorVertex{
            glm::vec3(-5.0f, 0.0f, -5.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(1.0f),
            glm::vec2(0.0f, -1.0f)
        },

        EditorVertex{
            glm::vec3(5.0f, 0.0f, -5.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(1.0f),
            glm::vec2(-2.0f, -1.0f)
        },

        EditorVertex{
            glm::vec3(5.0f, 5.0f, -5.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(1.0f),
            glm::vec2(-2.0f, -2.0f)
        },

        EditorVertex{
            glm::vec3(-5.0f, 5.0f, -5.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(1.0f),
            glm::vec2(0.0f, -2.0f)
        },


        // ----------------------------------------------------
        // Wall 2
        // ----------------------------------------------------

        EditorVertex{
            glm::vec3(5.0f, 0.0f, -5.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f),
            glm::vec2(0.0f, 0.0f)
        },

        EditorVertex{
            glm::vec3(5.0f, 0.0f, 5.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f),
            glm::vec2(-2.0f, 0.0f)
        },

        EditorVertex{
            glm::vec3(5.0f, 5.0f, 5.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f),
            glm::vec2(-2.0f, -1.0f)
        },

        EditorVertex{
            glm::vec3(5.0f, 5.0f, -5.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f),
            glm::vec2(0.0f, -1.0f)
        }
    };


    // ========================================================
    // Materials
    // ========================================================

    std::filesystem::path projectRoot =
        GetParentFolderName();

    data.materials.push_back(
        EditorMaterial{
            "Wall01",
            (projectRoot / "Game/Textures/Walls/Wall01.png").string()
        }
    );

    data.materials.push_back(
        EditorMaterial{
            "Floor01",
            (projectRoot / "Game/Textures/Floors/Floor01.png").string()
        }
    );


    // ========================================================
    // EDGES
    // ========================================================

    // --------------------------------------------------------
    // Floor
    //
    //      0 -------- 1
    //      |          |
    //      |          |
    //      |          |
    //      3 -------- 2
    // --------------------------------------------------------

    unsigned int floorEdge0 =
        map.AddEdge(0, 1);

    unsigned int floorEdge1 =
        map.AddEdge(1, 2);

    unsigned int floorEdge2 =
        map.AddEdge(2, 3);

    unsigned int floorEdge3 =
        map.AddEdge(3, 0);


    // --------------------------------------------------------
    // Wall 1
    //
    //      4 -------- 5
    //      |          |
    //      |          |
    //      7 -------- 6
    // --------------------------------------------------------

    unsigned int wall1Edge0 =
        map.AddEdge(4, 5);

    unsigned int wall1Edge1 =
        map.AddEdge(5, 6);

    unsigned int wall1Edge2 =
        map.AddEdge(6, 7);

    unsigned int wall1Edge3 =
        map.AddEdge(7, 4);


    // --------------------------------------------------------
    // Wall 2
    //
    //      8 -------- 9
    //      |          |
    //      |          |
    //      11 ------- 10
    // --------------------------------------------------------

    unsigned int wall2Edge0 =
        map.AddEdge(8, 9);

    unsigned int wall2Edge1 =
        map.AddEdge(9, 10);

    unsigned int wall2Edge2 =
        map.AddEdge(10, 11);

    unsigned int wall2Edge3 =
        map.AddEdge(11, 8);


    // ========================================================
    // Surfaces
    // ========================================================

    EditorSurface floor01;
    EditorSurface wall01;
    EditorSurface wall02;

    // floor01.vertices = {map.GetVertex(0, data), map.GetVertex(1, data), map.GetVertex(2, data), map.GetVertex(3, data)};

    // wall01.vertices = {map.GetVertex(4, data), map.GetVertex(5, data), map.GetVertex(6, data), map.GetVertex(7, data)};

    // wall02.vertices = {map.GetVertex(8, data), map.GetVertex(9, data), map.GetVertex(10, data), map.GetVertex(11, data)};

    floor01.material = 1;

    wall01.material = 0;

    wall02.material = 0;


    // --------------------------------------------------------
    // Surface -> edges
    // --------------------------------------------------------

    floor01.edges =
    {
        floorEdge0,
        floorEdge1,
        floorEdge2,
        floorEdge3
    };


    wall01.edges =
    {
        wall1Edge0,
        wall1Edge1,
        wall1Edge2,
        wall1Edge3
    };


    wall02.edges =
    {
        wall2Edge0,
        wall2Edge1,
        wall2Edge2,
        wall2Edge3
    };


    // --------------------------------------------------------
    // Surface -> triangles
    //
    // We keep these for rendering.
    // Edges are for topology.
    // --------------------------------------------------------

    floor01.indices =
    {
        0, 2, 1,
        0, 3, 2
    };

    wall01.indices =
    {
        4, 5, 6,
        4, 6, 7
    };

    wall02.indices =
    {
        8, 9, 10,
        8, 10, 11
    };


    // --------------------------------------------------------
    // Add surfaces
    // --------------------------------------------------------

    data.surfaces.push_back(
        floor01
    );

    data.surfaces.push_back(
        wall01
    );

    data.surfaces.push_back(
        wall02
    );


    // ========================================================
    // Entity
    // ========================================================

    data.entities.push_back(
        EditorEntity{
            1,

            glm::vec3(
                0.0f,
                1.0f,
                0.0f
            ),

            glm::vec3(0.0f),

            glm::vec3(1.0f)
        }
    );

    EditorLight light;

    light.type =
        EditorLightType::Spot;

    light.position =
        glm::vec3(0.0f, 5.0f, 0.0f);

    light.direction =
        glm::vec3(0.0f, -1.0f, 0.0f);

    light.ambient =
        glm::vec3(0.2f);

    light.diffuse =
        glm::vec3(1.0f);

    light.constant =
        1.0f;

    light.linear =
        0.7f;

    light.quadratic =
        3.0f;

    light.innerCone =
        0.95f;

    light.outerCone =
        0.90f;

    data.lights.push_back(light);

    map.selectedSurface = 0;
    map.RotateSelectedSurface((glm::vec3){-360.0f, 0.0f, 0.0f});
}

// ============================================================
// Cleanup
// ============================================================

void deInit(RenderTexture2D rTexture)
{
    rlImGuiShutdown();

    UnloadRenderTexture(rTexture);

    CloseWindow();
}

// ============================================================
// Global editor state
// ============================================================

bool running = false;

bool showPreferences = false;

Map currentMap;

std::string currentFilename;

int selectedMaterial = -1;


// ============================================================
// Camera
// ============================================================

Vector3 GetCameraForward(
    float yaw,
    float pitch
)
{
    Vector3 forward;

    forward.x =
        cosf(pitch) * sinf(yaw);

    forward.y =
        sinf(pitch);

    forward.z =
        cosf(pitch) * cosf(yaw);

    return Vector3Normalize(
        forward
    );
}


// ============================================================
// Main
// ============================================================

int main(
    int argc,
    char* argv[]
)
{
    // ========================================================
    // Open map passed through command line
    // ========================================================

    if (argc > 1)
    {
        std::string filename =
            argv[1];

        if (currentMap.Load(filename))
        {
            currentFilename =
                filename;

            std::cout
                << "Opened: "
                << filename
                << '\n';
        }
    }


    selectedMaterial = -1;


    // ========================================================
    // Raylib
    // ========================================================

    SetTraceLogLevel(
        LOG_ERROR
    );

    InitWindow(
        800,
        600,
        "Level Editor"
    );


    // ========================================================
    // Viewport render texture
    // ========================================================

    RenderTexture2D viewportTexture =
        LoadRenderTexture(
            800,
            600
        );


    // ========================================================
    // Camera
    // ========================================================

    Camera3D camera = {};

    camera.position =
        Vector3{
            10.0f,
            8.0f,
            10.0f
        };

    camera.target =
        Vector3{
            0.0f,
            0.0f,
            0.0f
        };

    camera.up =
        Vector3{
            0.0f,
            1.0f,
            0.0f
        };

    camera.fovy =
        60.0f;

    camera.projection =
        CAMERA_PERSPECTIVE;


    float movementSpeed =
        5.0f;

    float cameraYaw =
        -45.0f * DEG2RAD;

    float cameraPitch =
        -25.0f * DEG2RAD;


    // ========================================================
    // ImGui
    // ========================================================

    rlImGuiSetup(true);

    running = true;


    Vector2 mousePos;


    // ========================================================
    // Main loop
    // ========================================================

    while (
        running &&
        !WindowShouldClose()
    )
    {
        // ====================================================
        // Render 3D viewport
        // ====================================================

        BeginTextureMode(
            viewportTexture
        );

        ClearBackground(
            DARKGRAY
        );

        BeginMode3D(
            camera
        );

        DrawGrid(
            500,
            1.0f
        );


        // ----------------------------------------------------
        // Map
        // ----------------------------------------------------

        currentMap.Draw();

        currentMap.DrawEntities(camera);

        currentMap.DrawLights(camera);

        // ----------------------------------------------------
        // Topology
        // ----------------------------------------------------

        currentMap.DrawEdges();
        
        // ----------------------------------------------------
        // Selection
        // ----------------------------------------------------

        currentMap.DrawSelection();

        currentMap.DrawSurfaceVertexSelection();


        // ----------------------------------------------------
        // Unused vertices
        // ----------------------------------------------------

        currentMap.DrawOrphanedVertices();


        EndMode3D();

        EndTextureMode();


        // ====================================================
        // Begin drawing
        // ====================================================

        BeginDrawing();

        ClearBackground(
            DARKGRAY
        );

        rlImGuiBegin();


        // ====================================================
        // Main Menu
        // ====================================================

        if (ImGui::BeginMainMenuBar())
        {
            // ==================================================
            // File
            // ==================================================

            if (ImGui::BeginMenu("File"))
            {
                // ------------------------------------------------
                // New
                // ------------------------------------------------

                if (ImGui::MenuItem("New"))
                {
                    currentMap.New(
                        "Untitled Map"
                    );

                    CreateDefaultMap(
                        currentMap
                    );

                    currentMap.BuildRenderMeshes();

                    currentFilename.clear();

                    std::cout
                        << "Created new map.\n";
                }


                // ------------------------------------------------
                // Open
                // ------------------------------------------------

                if (ImGui::MenuItem("Open"))
                {
                    std::string filename =
                        OpenMapDialog();

                    if (!filename.empty())
                    {
                        if (currentMap.Load(
                                filename
                            ))
                        {
                            currentFilename =
                                filename;

                            std::cout
                                << "Opened: "
                                << filename
                                << '\n';
                        }
                    }
                }


                // ------------------------------------------------
                // Save
                // ------------------------------------------------

                if (ImGui::MenuItem("Save"))
                {
                    if (currentFilename.empty())
                    {
                        std::string filename =
                            SaveMapDialog();

                        if (!filename.empty())
                        {
                            if (currentMap.Save(
                                    filename
                                ))
                            {
                                currentFilename =
                                    filename;
                            }
                        }
                    }
                    else
                    {
                        currentMap.Save(
                            currentFilename
                        );
                    }
                }


                // ------------------------------------------------
                // Save As
                // ------------------------------------------------

                if (ImGui::MenuItem(
                        "Save As..."
                    ))
                {
                    std::string filename =
                        SaveMapDialog();

                    if (!filename.empty())
                    {
                        if (currentMap.Save(
                                filename
                            ))
                        {
                            currentFilename =
                                filename;
                        }
                    }
                }


                ImGui::Separator();


                // ------------------------------------------------
                // Quit
                // ------------------------------------------------

                if (ImGui::MenuItem("Quit"))
                {
                    running = false;
                }


                ImGui::EndMenu();
            }


            // ==================================================
            // Preferences
            // ==================================================

            if (ImGui::BeginMenu(
                    "Preferences"
                ))
            {
                if (ImGui::MenuItem(
                        "Open Preferences"
                    ))
                {
                    showPreferences =
                        true;
                }

                ImGui::EndMenu();
            }


            ImGui::EndMainMenuBar();
        }


        // ====================================================
        // Preferences window
        // ====================================================

        if (showPreferences)
        {
            ImGui::SetNextWindowSize(
                ImVec2(
                    640,
                    360
                ),
                ImGuiCond_Always
            );

            ImGui::Begin(
                "Preferences",
                nullptr,
                ImGuiWindowFlags_NoResize
            );


            ImGui::DragFloat(
                "Viewport Movement Speed",
                &movementSpeed,
                movementSpeed,
                0.001f,
                100.0f
            );


            ImGui::DragFloat(
                "Viewport Surface Highlight",
                &lineWidth,
                lineWidth,
                0.00001f,
                10.0f
            );


            ImGui::DragFloat(
                "Viewport Vertex Highlight",
                &vertexSelectionWidth,
                vertexSelectionWidth,
                0.00001f,
                10.0f
            );


            ImGui::Separator();


            if (ImGui::Button("Close"))
            {
                showPreferences =
                    false;
            }


            ImGui::End();
        }


        // ====================================================
        // Viewport
        // ====================================================

        if (ImGui::Begin("Viewport"))
        {
            ImVec2 viewportSize =
                ImGui::GetContentRegionAvail();


            if (
                viewportSize.x > 0 &&
                viewportSize.y > 0
            )
            {
                ImVec2 viewportPos =
                    ImGui::GetCursorScreenPos();


                ImGui::Image(
                    (ImTextureID)(uintptr_t)
                        viewportTexture.texture.id,

                    viewportSize,

                    ImVec2(0, 1),

                    ImVec2(1, 0)
                );


                bool viewportHovered =
                    ImGui::IsItemHovered();


                float speed =
                    movementSpeed;


                if (IsKeyDown(
                        KEY_LEFT_SHIFT
                    ))
                {
                    speed *= 3.0f;
                }


                float dt =
                    GetFrameTime();


                // ==================================================
                // Camera forward
                // ==================================================

                Vector3 forward =
                {
                    cosf(cameraPitch) *
                        sinf(cameraYaw),

                    sinf(cameraPitch),

                    cosf(cameraPitch) *
                        cosf(cameraYaw)
                };


                forward =
                    Vector3Normalize(
                        forward
                    );


                Vector3 right =
                    Vector3Normalize(
                        Vector3CrossProduct(
                            forward,
                            camera.up
                        )
                    );


                camera.target =
                    Vector3Add(
                        camera.position,
                        forward
                    );


                // ==================================================
                // Viewport input
                // ==================================================

                if (viewportHovered)
                {
                    mousePos =
                        GetMousePosition();


                    Vector2 viewportMouse =
                    {
                        mousePos.x -
                            viewportPos.x,

                        mousePos.y -
                            viewportPos.y
                    };


                    Vector2 rayMouse =
                    {
                        viewportMouse.x *
                            (1280.0f /
                             viewportSize.x),

                        viewportMouse.y *
                            (720.0f /
                             viewportSize.y)
                    };


                    Ray ray =
                        GetMouseRay(
                            rayMouse,
                            camera
                        );


                    // ==================================================
                    // Hover surface
                    // ==================================================

                    int hoveredSurface =
                        currentMap.PickSurface(
                            ray
                        );

                    currentMap.SetHoveredSurface(
                        hoveredSurface
                    );

                    int hoveredEntity =
                        currentMap.PickEntity(
                            ray
                        );

                    currentMap.SetHoveredEntity(
                        hoveredEntity
                    );


                    // ==================================================
                    // Hover vertex
                    // ==================================================

                    int hoveredVertex =
                        currentMap.PickVertex(
                            ray
                        );

                    currentMap.SetHoveredVertex(
                        hoveredVertex
                    );

                    int hoveredEdge =
                        currentMap.PickEdge(
                            ray
                        );

                    currentMap.SetHoveredEdge(
                        hoveredEdge
                    );

                    int hoveredLight =
                        currentMap.PickLight(
                            ray
                        );

                    currentMap.SetHoveredLight(
                        hoveredLight
                    );                    

                    // ==================================================
                    // Mouse selection
                    // ==================================================

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    {
                        int light = currentMap.PickLight(ray);

                        if (light >= 0)
                        {
                            currentMap.SetSelectedVertex(-1);
                            currentMap.SetSelectedEdge(-1);
                            currentMap.SetSelectedSurface(-1);
                            currentMap.SetSelectedLight(light);
                            currentMap.SetSelectedEntity(-1);
                            
                            std::cout << "Picked light: "
                                << light
                                << '\n';
                        }
                        else
                        {
                            int vertex = currentMap.PickVertex(ray);

                            if (vertex >= 0)
                            {
                                currentMap.SetSelectedVertex(vertex);
                                currentMap.SetSelectedEdge(-1);
                                currentMap.SetSelectedSurface(-1);
                                currentMap.SetSelectedLight(-1);
                                currentMap.SetSelectedEntity(-1);
                            }
                            else
                            {
                                int edge = currentMap.PickEdge(ray);

                                if (edge >= 0)
                                {
                                    currentMap.SetSelectedVertex(-1);
                                    currentMap.SetSelectedEdge(edge);
                                    currentMap.SetSelectedSurface(-1);
                                    currentMap.SetSelectedLight(-1);
                                    currentMap.SetSelectedEntity(-1);
                                }
                                else
                                {
                                    int surface = currentMap.PickSurface(ray);

                                    if (surface >= 0)
                                    {
                                        currentMap.SetSelectedVertex(-1);
                                        currentMap.SetSelectedEdge(-1);
                                        currentMap.SetSelectedSurface(surface);
                                        currentMap.SetSelectedLight(-1);
                                        currentMap.SetSelectedEntity(-1);
                                    }
                                    else
                                    {
                                        int entity = currentMap.PickEntity(ray);

                                        currentMap.SetSelectedVertex(-1);
                                        currentMap.SetSelectedEdge(-1);
                                        currentMap.SetSelectedSurface(-1);
                                        currentMap.SetSelectedLight(-1);
                                        currentMap.SetSelectedEntity(entity);
                                    }
                                }
                            }
                        }
                    }


                    // ==================================================
                    // Move selected vertex
                    // ==================================================

                    const float vertexMoveAmount =
                        0.25f;


                    if (IsKeyPressed(KEY_RIGHT))
                    {
                        currentMap.MoveSelectedVertex(
                            {
                                vertexMoveAmount,
                                0.0f,
                                0.0f
                            }
                        );
                        currentMap.MoveSelectedSurface(
                            {
                                vertexMoveAmount,
                                0.0f,
                                0.0f
                            }
                        );
                    }


                    if (IsKeyPressed(KEY_LEFT))
                    {
                        currentMap.MoveSelectedVertex(
                            {
                                -vertexMoveAmount,
                                0.0f,
                                0.0f
                            }
                        );
                        currentMap.MoveSelectedSurface(
                            {
                                -vertexMoveAmount,
                                0.0f,
                                0.0f
                            }
                        );
                    }


                    if (IsKeyPressed(KEY_DOWN))
                    {
                        currentMap.MoveSelectedVertex(
                            {
                                0.0f,
                                0.0f,
                                vertexMoveAmount
                            }
                        );
                        currentMap.MoveSelectedSurface(
                            {
                                0.0f,
                                0.0f,
                                vertexMoveAmount
                            }
                        );
                    }


                    if (IsKeyPressed(KEY_UP))
                    {
                        currentMap.MoveSelectedVertex(
                            {
                                0.0f,
                                0.0f,
                                -vertexMoveAmount
                            }
                        );
                        currentMap.MoveSelectedSurface(
                            {
                                0.0f,
                                0.0f,
                                -vertexMoveAmount
                            }
                        );
                    }


                    if (IsKeyPressed(KEY_Z))
                    {
                        currentMap.MoveSelectedVertex(
                            {
                                0.0f,
                                vertexMoveAmount,
                                0.0f
                            }
                        );
                        currentMap.MoveSelectedSurface(
                            {
                                0.0f,
                                vertexMoveAmount,
                                0.0f
                            }
                        );
                    }


                    if (IsKeyPressed(KEY_X))
                    {
                        currentMap.MoveSelectedVertex(
                            {
                                0.0f,
                                -vertexMoveAmount,
                                0.0f
                            }
                        );
                        currentMap.MoveSelectedSurface(
                            {
                                0.0f,
                                -vertexMoveAmount,
                                0.0f
                            }
                        );
                    }


                    // ==================================================
                    // Delete
                    // ==================================================

                    if (IsKeyPressed(KEY_DELETE))
                    {
                        if (
                            currentMap.GetSelectedVertex()
                            >= 0
                        )
                        {
                            currentMap.DeleteSelectedVertex();
                        }
                        else if (
                            currentMap.GetSelectedSurface()
                            >= 0
                        )
                        {
                            currentMap.DeleteSelectedSurface();
                        }
                    }


                    // ==================================================
                    // Camera rotation
                    // ==================================================

                    if (
                        IsMouseButtonDown(
                            MOUSE_BUTTON_RIGHT
                        )
                    )
                    {
                        Vector2 mouseDelta =
                            GetMouseDelta();


                        const float sensitivity =
                            0.003f;


                        cameraYaw -=
                            mouseDelta.x *
                            sensitivity;


                        cameraPitch -=
                            mouseDelta.y *
                            sensitivity;
                    }


                    // ==================================================
                    // Camera movement
                    // ==================================================

                    if (IsKeyDown(KEY_W))
                    {
                        camera.position =
                            Vector3Add(
                                camera.position,

                                Vector3Scale(
                                    forward,
                                    speed * dt
                                )
                            );
                    }


                    if (IsKeyDown(KEY_S))
                    {
                        camera.position =
                            Vector3Subtract(
                                camera.position,

                                Vector3Scale(
                                    forward,
                                    speed * dt
                                )
                            );
                    }


                    if (IsKeyDown(KEY_A))
                    {
                        camera.position =
                            Vector3Subtract(
                                camera.position,

                                Vector3Scale(
                                    right,
                                    speed * dt
                                )
                            );
                    }


                    if (IsKeyDown(KEY_D))
                    {
                        camera.position =
                            Vector3Add(
                                camera.position,

                                Vector3Scale(
                                    right,
                                    speed * dt
                                )
                            );
                    }


                    if (IsKeyDown(KEY_E))
                    {
                        camera.position.y +=
                            speed * dt;
                    }


                    if (IsKeyDown(KEY_Q))
                    {
                        camera.position.y -=
                            speed * dt;
                    }


                    // ==================================================
                    // Undo
                    // ==================================================

                    if (
                        (
                            IsKeyDown(
                                KEY_LEFT_CONTROL
                            ) ||

                            IsKeyDown(
                                KEY_RIGHT_CONTROL
                            )
                        ) &&

                        IsKeyPressed(KEY_Z)
                    )
                    {
                        currentMap.Undo();
                    }
                }
            }


            ImGui::End();
        }

        if (ImGui::Begin("SelectedObject"))
        {
            if (currentMap.selectedVertex != -1)
            {
                EditorVertex selectedVertex = currentMap.GetVertex(currentMap.selectedVertex, currentMap.GetData());

                glm::vec3 selectedVertexPos = selectedVertex.position;

                ImGui::InputFloat("Object X", &selectedVertexPos.x,  0.25f, 1.0f, "%.3f");
                ImGui::InputFloat("Object Y", &selectedVertexPos.y,  0.25f, 1.0f, "%.3f");
                ImGui::InputFloat("Object Z", &selectedVertexPos.z,  0.25f, 1.0f, "%.3f");

                currentMap.MoveSelectedVertex(selectedVertexPos - selectedVertex.position);

            }
            else if (currentMap.selectedEdge != -1)
            {
                EditorData& data = currentMap.GetData();

                EditorEdge& edge =
                    data.edges[currentMap.selectedEdge];

                std::vector<unsigned int> vertexIndices =
                    currentMap.GetEdgeVertices(
                        edge,
                        data
                    );

                glm::vec3 selectedEdgePos = glm::vec3(0.0f);

                int validVertexCount = 0;

                for (unsigned int vertexIndex : vertexIndices)
                {
                    if (vertexIndex >= data.vertices.size())
                        continue;

                    selectedEdgePos +=
                        data.vertices[vertexIndex].position;

                    validVertexCount++;
                }

                if (validVertexCount == 0)
                {
                    ImGui::Text("Edge has no valid vertices.");
                }
                else
                {
                    selectedEdgePos /=
                        static_cast<float>(validVertexCount);

                    glm::vec3 selectedEdgeOGPos =
                        selectedEdgePos;

                    ImGui::InputFloat("Object X", &selectedEdgePos.x,  0.25f, 1.0f, "%.3f");
                    ImGui::InputFloat("Object Y", &selectedEdgePos.y,  0.25f, 1.0f, "%.3f");
                    ImGui::InputFloat("Object Z", &selectedEdgePos.z,  0.25f, 1.0f, "%.3f");

                    currentMap.MoveSelectedEdge(selectedEdgePos - selectedEdgeOGPos);
                }

            }
            else if (currentMap.selectedSurface != -1)
            {
                EditorData& data = currentMap.GetData();

                EditorSurface& surface =
                    data.surfaces[currentMap.selectedSurface];

                // ----------------------------------------------------
                // Get the actual master vertices belonging to surface
                // ----------------------------------------------------

                std::vector<unsigned int> vertexIndices =
                    currentMap.GetSurfaceVertices(
                        surface,
                        data
                    );

                // ----------------------------------------------------
                // Calculate the center of the surface
                // ----------------------------------------------------

                glm::vec3 selectedSurfacePos =
                    glm::vec3(0.0f);

                int validVertexCount = 0;

                for (unsigned int vertexIndex : vertexIndices)
                {
                    if (vertexIndex >= data.vertices.size())
                        continue;

                    selectedSurfacePos +=
                        data.vertices[vertexIndex].position;

                    validVertexCount++;
                }

                // ----------------------------------------------------
                // Only display position if we have valid vertices
                // ----------------------------------------------------

                if (validVertexCount > 0)
                {
                    selectedSurfacePos /=
                        static_cast<float>(validVertexCount);

                    glm::vec3 selectedSurfaceOGPos =
                        selectedSurfacePos;

                    // ------------------------------------------------
                    // Position X
                    // ------------------------------------------------

                    if (ImGui::InputFloat(
                        "Object X",
                        &selectedSurfacePos.x,
                        0.25f,
                        1.0f, "%.3f"
                    ))
                    {
                        glm::vec3 delta =
                        {
                            selectedSurfacePos.x -
                            selectedSurfaceOGPos.x,

                            0.0f,
                            0.0f
                        };

                        currentMap.MoveSelectedSurface(delta);
                    }

                    // ------------------------------------------------
                    // Position Y
                    // ------------------------------------------------

                    if (ImGui::InputFloat(
                        "Object Y",
                        &selectedSurfacePos.y,
                        0.25f,
                        1.0f, "%.3f"
                    ))
                    {
                        glm::vec3 delta =
                        {
                            0.0f,

                            selectedSurfacePos.y -
                            selectedSurfaceOGPos.y,

                            0.0f
                        };

                        currentMap.MoveSelectedSurface(delta);
                    }

                    // ------------------------------------------------
                    // Position Z
                    // ------------------------------------------------

                    if (ImGui::InputFloat(
                        "Object Z",
                        &selectedSurfacePos.z,
                        0.25f,
                        1.0f, "%.3f"
                    ))
                    {
                        glm::vec3 delta =
                        {
                            0.0f,
                            0.0f,

                            selectedSurfacePos.z -
                            selectedSurfaceOGPos.z
                        };

                        currentMap.MoveSelectedSurface(delta);
                    }

                    ImGui::Separator();

                    glm::vec3 oldRotation = surface.rotation;

                    if (ImGui::InputFloat(
                        "Rotation X",
                        &surface.rotation.x,
                        1.0f,
                        1.0f, "%.3f"
                    ))
                    {
                        float delta = surface.rotation.x - oldRotation.x;

                        currentMap.RotateSelectedSurface(
                            glm::vec3(delta, 0.0f, 0.0f)
                        );
                    }

                    if (ImGui::InputFloat(
                        "Rotation Y",
                        &surface.rotation.y,
                        1.0f,
                        1.0f, "%.3f"
                    ))
                    {
                        float delta = surface.rotation.y - oldRotation.y;

                        currentMap.RotateSelectedSurface(
                            glm::vec3(0.0f, delta, 0.0f)
                        );
                    }

                    if (ImGui::InputFloat(
                        "Rotation Z",
                        &surface.rotation.z,
                        1.0f,
                        1.0f, "%.3f"
                    ))
                    {
                        float delta = surface.rotation.z - oldRotation.z;

                        currentMap.RotateSelectedSurface(
                            glm::vec3(0.0f, 0.0f, delta)
                        );
                    }

                    ImGui::Separator();

                    glm::vec3 oldScale = surface.scale;

                    if (ImGui::InputFloat(
                        "Scale X",
                        &surface.scale.x,
                        0.25f,
                        1.0f, "%.3f"
                    ))
                    {
                        float factor =
                            surface.scale.x / oldScale.x;

                        currentMap.ScaleSelectedSurface(
                            glm::vec3(factor, 1.0f, 1.0f)
                        );
                    }

                    if (ImGui::InputFloat(
                        "Scale Y",
                        &surface.scale.y,
                        0.25f,
                        1.0f, "%.3f"
                    ))
                    {
                        float factor =
                            surface.scale.y / oldScale.y;

                        currentMap.ScaleSelectedSurface(
                            glm::vec3(1.0f, factor, 1.0f)
                        );
                    }

                    if (ImGui::InputFloat(
                        "Scale Z",
                        &surface.scale.z,
                        0.25f,
                        1.0f, "%.3f"
                    ))
                    {
                        float factor =
                            surface.scale.z / oldScale.z;

                        currentMap.ScaleSelectedSurface(
                            glm::vec3(1.0f, 1.0f, factor)
                        );
                    }
                }
                else
                {
                    ImGui::Text("Surface has no valid vertices.");
                }
            }
            else if (currentMap.selectedLight != -1)
            {   
                EditorData& mapData = currentMap.GetData();

                EditorLight& selectedLight = mapData.lights[currentMap.selectedLight];

                glm::vec3 lightPos = selectedLight.position;

                ImGui::InputFloat("Object X", &lightPos.x,  0.25f, 1.0f, "%.3f");
                ImGui::InputFloat("Object Y", &lightPos.y,  0.25f, 1.0f, "%.3f");
                ImGui::InputFloat("Object Z", &lightPos.z,  0.25f, 1.0f, "%.3f");

                currentMap.MoveSelectedLight(lightPos - selectedLight.position);

                ImGui::Separator();

                std::string lightData = "CLOSE";

                static bool open = false;

                if (ImGui::Button(
                    open ? "CLOSE" : "OPEN"
                ))
                {
                    open = !open;
                }


                if (open)
                {
                    const char* lightTypes[] =
                    {
                        "Directional",
                        "Point",
                        "Spot"
                    };


                    int selectedLightType =
                        static_cast<int>(
                            selectedLight.type
                        );


                    if (ImGui::Combo(
                        "Light Type",
                        &selectedLightType,
                        lightTypes,
                        IM_ARRAYSIZE(lightTypes)
                    ))
                    {
                        selectedLight.type =
                            static_cast<EditorLightType>(
                                selectedLightType
                            );
                    }


                    // ====================================================
                    // Directional / Spot
                    // ====================================================

                    if (
                        selectedLight.type ==
                            EditorLightType::Directional ||

                        selectedLight.type ==
                            EditorLightType::Spot
                    )
                    {
                        ImGui::DragFloat3(
                            "Direction",
                            glm::value_ptr(
                                selectedLight.direction
                            ),
                            0.01f
                        );
                    }


                    // ====================================================
                    // Point / Spot
                    // ====================================================

                    if (
                        selectedLight.type ==
                            EditorLightType::Point ||

                        selectedLight.type ==
                            EditorLightType::Spot
                    )
                    {
                        ImGui::DragFloat(
                            "Constant",
                            &selectedLight.constant,
                            0.01f
                        );

                        ImGui::DragFloat(
                            "Linear",
                            &selectedLight.linear,
                            0.01f
                        );

                        ImGui::DragFloat(
                            "Quadratic",
                            &selectedLight.quadratic,
                            0.01f
                        );
                    }


                    // ====================================================
                    // Spot
                    // ====================================================

                    if (
                        selectedLight.type ==
                        EditorLightType::Spot
                    )
                    {
                        ImGui::DragFloat(
                            "Inner Cone",
                            &selectedLight.innerCone,
                            0.01f,
                            0.0f,
                            1.0f
                        );

                        ImGui::DragFloat(
                            "Outer Cone",
                            &selectedLight.outerCone,
                            0.01f,
                            0.0f,
                            1.0f
                        );
                    }
                }

                // if (ImGui::Combo("Light Type", &selectedLightType, lightTypes, IM_ARRAYSIZE(lightTypes)))
                // {
                // 
                // }
            }
        
            /*else if (currentMap.selectedVertex == -1 && currentMap.selectedSurface != -1 && currentMap.selectedEdge == -1)
            {
                EditorSurface selectedSurface = currentMap.GetSurface(currentMap.selectedSurface, currentMap.GetData());

                glm::vec3 selectedSurfacePos;
                glm::vec3 selectedSurfaceOGPos;

                std::vector<float> vertexPosX;
                std::vector<float> vertexPosY;
                std::vector<float> vertexPosZ;
                
                for (int i = 0; i < selectedSurface.vertices.size(); i++)
                {
                    vertexPosX.push_back(selectedSurface.vertices[i].position.x);
                    vertexPosY.push_back(selectedSurface.vertices[i].position.y);
                    vertexPosZ.push_back(selectedSurface.vertices[i].position.z);
                }

                selectedSurfacePos.x = average(vertexPosX);
                selectedSurfacePos.y = average(vertexPosY);
                selectedSurfacePos.z = average(vertexPosZ);

                selectedSurfaceOGPos.x = average(vertexPosX);
                selectedSurfaceOGPos.y = average(vertexPosY);
                selectedSurfaceOGPos.z = average(vertexPosZ);
                
                ImGui::DragFloat("Object X", &selectedSurfacePos.x,  0.25f, selectedSurfacePos.x - 5.0f, selectedSurfacePos.x + 5.0f, "%.3f");
                ImGui::DragFloat("Object Y", &selectedSurfacePos.y,  0.25f, selectedSurfacePos.y - 5.0f, selectedSurfacePos.y + 5.0f, "%.3f");
                ImGui::DragFloat("Object Z", &selectedSurfacePos.z,  0.25f, selectedSurfacePos.z - 5.0f, selectedSurfacePos.z + 5.0f, "%.3f");

                currentMap.MoveSelectedSurface(selectedSurfacePos - selectedSurfaceOGPos);
            }*/

            ImGui::End();
        }

        // ====================================================
        // Map information
        // ====================================================

        ImGui::Begin("Map");


        EditorData& mapData =
            currentMap.GetData();


        ImGui::Text(
            "Name: %s",
            mapData.name.c_str()
        );


        if (currentFilename.empty())
        {
            ImGui::Text(
                "File: <unsaved>"
            );
        }
        else
        {
            ImGui::Text(
                "File: %s",
                currentFilename.c_str()
            );
        }


        ImGui::Separator();


        ImGui::Text(
            "Vertices: %zu",
            mapData.vertices.size()
        );


        ImGui::Text(
            "Edges: %zu",
            mapData.edges.size()
        );


        ImGui::Text(
            "Surfaces: %zu",
            mapData.surfaces.size()
        );


        ImGui::Text(
            "Entities: %zu",
            mapData.entities.size()
        );


        ImGui::End();


        // ====================================================
        // Materials
        // ====================================================

        if (ImGui::Begin("Materials"))
        {

            for (
                size_t i = 0;
                i < mapData.materials.size();
                i++
            )
            {
                EditorMaterial& material =
                    mapData.materials[i];


                std::string label =
                    std::to_string(i) +
                    ": " +
                    material.name;


                if (
                    ImGui::Selectable(
                        label.c_str(),

                        selectedMaterial ==
                            static_cast<int>(i)
                    )
                )
                {
                    selectedMaterial =
                        static_cast<int>(i);
                }
            }


            ImGui::Separator();


            if (
                selectedMaterial >= 0 &&

                selectedMaterial <
                    static_cast<int>(
                        mapData.materials.size()
                    )
            )
            {
                EditorMaterial& material =
                    mapData.materials[
                        selectedMaterial
                    ];


                ImGui::Text(
                    "Selected:"
                );


                ImGui::Text(
                    "%s",
                    material.name.c_str()
                );


                ImGui::Text(
                    "%s",
                    material.texture.c_str()
                );
            }
            else
            {
                ImGui::Text(
                    "No material selected."
                );
            }


            ImGui::End();
        }

        // ====================================================
        // Finish ImGui
        // ====================================================

        rlImGuiEnd();

        EndDrawing();
    }


    // ========================================================
    // Cleanup
    // ========================================================

    deInit(
        viewportTexture
    );


    return 0;
}