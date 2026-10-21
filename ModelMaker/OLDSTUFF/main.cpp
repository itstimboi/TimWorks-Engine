// #include <windows.h>
// #include <commdlg.h>

// #ifdef LoadImage
// #undef LoadImage
// #endif

#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <raylib/rlgl.h>

#include "rlImGui/rlImGui.h"
#include "ImGUI/imgui.h"

#include "Model/TWModel.h"
#include "Model/TwMdlIO.h"
#include "FileDialogs/FileDialog.h"
#include "Base64/Base64.h"
#include "Textures/TextureMaker.h"
#include "Tools/PrimitiveMaker.h"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include <utility>
#include <vector>


// ============================================================
// Global Model State
// ============================================================

static TW::TWModel model;

static std::string currentFile;

static int selectedMaterial = -1;
static int selectedFrame = 0;
static int selectedSurface = -1;
static int selectedAnimation = -1;

static bool showDemoWindow = false;

static std::string errorMessage;

static TW::TextureMaker textureMaker;


// ============================================================
// Material Preview
// ============================================================

struct MaterialPreview
{
    Texture2D texture = {};
    bool loaded = false;

    std::string base64Data;
};

static std::vector<MaterialPreview> materialPreviews;

static bool LoadMaterialPreview(
    int index
)
{
    if (index < 0)
        return false;

    if (index >= static_cast<int>(model.materials.size()))
        return false;

    // Make sure the preview array is large enough
    if (index >= static_cast<int>(materialPreviews.size()))
    {
        materialPreviews.resize(
            model.materials.size()
        );
    }

    MaterialPreview& preview =
        materialPreviews[index];

    const TW::ModelMaterial& material =
        model.materials[index];

    // If this preview is already loaded with
    // exactly the same Base64 data, nothing to do.
    if (preview.loaded &&
        preview.base64Data == material.texture.data)
    {
        return true;
    }

    // Remove old texture
    if (preview.loaded)
    {
        UnloadTexture(preview.texture);

        preview.texture = {};
        preview.loaded = false;
    }

    preview.base64Data.clear();

    // No embedded texture
    if (!material.texture.embedded)
        return false;

    if (material.texture.data.empty())
        return false;

    // --------------------------------------------------------
    // Decode Base64
    // --------------------------------------------------------

    std::vector<unsigned char> bytes =
        TW::Base64Decode(
            material.texture.data
        );

    if (bytes.empty())
        return false;

    // --------------------------------------------------------
    // Determine Raylib image extension
    // --------------------------------------------------------

    const char* extension =
        GetRaylibImageExtension(
            material.texture.mime
        );

    if (!extension)
        return false;

    // --------------------------------------------------------
    // Load image
    // --------------------------------------------------------

    Image image =
        LoadImageFromMemory(
            extension,
            bytes.data(),
            static_cast<int>(bytes.size())
        );

    if (!image.data)
        return false;

    // --------------------------------------------------------
    // Convert to GPU texture
    // --------------------------------------------------------

    preview.texture =
        LoadTextureFromImage(image);

    UnloadImage(image);

    if (preview.texture.id == 0)
        return false;

    preview.loaded = true;

    preview.base64Data =
        material.texture.data;

    return true;
}

// ============================================================
// Material Preview Cleanup
// ============================================================

static void UnloadMaterialPreview(int index)
{
    if (index < 0)
        return;

    if (index >= static_cast<int>(materialPreviews.size()))
        return;

    MaterialPreview& preview =
        materialPreviews[index];

    if (preview.loaded)
    {
        UnloadTexture(preview.texture);

        preview.texture = {};
        preview.loaded = false;
    }

    preview.base64Data.clear();
}


// ============================================================
// MIME -> Raylib Extension
// ============================================================

static const char* GetRaylibImageExtension(
    const std::string& mime
)
{
    if (mime == "image/png")
        return ".png";

    if (mime == "image/jpeg")
        return ".jpg";

    if (mime == "image/jpg")
        return ".jpg";

    if (mime == "image/bmp")
        return ".bmp";

    if (mime == "image/tga")
        return ".tga";

    if (mime == "image/gif")
        return ".gif";

    if (mime == "image/psd")
        return ".psd";

    if (mime == "image/dds")
        return ".dds";

    if (mime == "image/pkm")
        return ".pkm";

    if (mime == "image/ktx")
        return ".ktx";

    return nullptr;
}


// ============================================================
// Update Material Preview
// ============================================================

static void UpdateMaterialPreview(int index)
{
    if (index < 0)
        return;

    if (index >= static_cast<int>(model.materials.size()))
        return;

    while (
        materialPreviews.size() <=
        static_cast<size_t>(index)
    )
    {
        materialPreviews.push_back({});
    }

    MaterialPreview& preview =
        materialPreviews[index];

    UnloadMaterialPreview(index);

    TW::ModelMaterial& material =
        model.materials[index];

    if (!material.texture.embedded)
        return;

    if (material.texture.data.empty())
        return;

    const char* extension =
        GetRaylibImageExtension(
            material.texture.mime
        );

    if (!extension)
    {
        TraceLog(
            LOG_ERROR,
            "Unsupported texture MIME type: %s",
            material.texture.mime.c_str()
        );

        return;
    }

    // --------------------------------------------------------
    // Decode Base64
    // --------------------------------------------------------

    std::vector<unsigned char> bytes =
        TW::Base64Decode(
            material.texture.data
        );

    TraceLog(
        LOG_INFO,
        "Base64 string length: %zu",
        material.texture.data.size()
    );

    TraceLog(
        LOG_INFO,
        "Decoded byte count: %zu",
        bytes.size()
    );

    if (bytes.empty())
    {
        TraceLog(
            LOG_ERROR,
            "Base64 decoding produced zero bytes."
        );

        return;
    }

    // --------------------------------------------------------
    // PNG Signature Check
    // --------------------------------------------------------

    if (material.texture.mime == "image/png")
    {
        if (bytes.size() < 8)
        {
            TraceLog(
                LOG_ERROR,
                "PNG data is smaller than the PNG signature."
            );

            return;
        }

        bool validPNG =
            bytes[0] == 0x89 &&
            bytes[1] == 0x50 &&
            bytes[2] == 0x4E &&
            bytes[3] == 0x47 &&
            bytes[4] == 0x0D &&
            bytes[5] == 0x0A &&
            bytes[6] == 0x1A &&
            bytes[7] == 0x0A;

        TraceLog(
            LOG_INFO,
            "PNG signature: %02X %02X %02X %02X %02X %02X %02X %02X",
            bytes[0],
            bytes[1],
            bytes[2],
            bytes[3],
            bytes[4],
            bytes[5],
            bytes[6],
            bytes[7]
        );

        if (!validPNG)
        {
            TraceLog(
                LOG_ERROR,
                "PNG signature is invalid."
            );

            return;
        }
    }

    // --------------------------------------------------------
    // Decode Image
    // --------------------------------------------------------

    Image image =
        LoadImageFromMemory(
            extension,
            bytes.data(),
            static_cast<int>(bytes.size())
        );

    if (!image.data)
    {
        TraceLog(
            LOG_ERROR,
            "Raylib could not load decoded texture data."
        );

        return;
    }

    // --------------------------------------------------------
    // Create GPU Texture
    // --------------------------------------------------------

    preview.texture =
        LoadTextureFromImage(image);

    UnloadImage(image);

    if (preview.texture.id == 0)
    {
        TraceLog(
            LOG_ERROR,
            "Could not create GPU texture."
        );

        return;
    }

    preview.loaded = true;

    preview.base64Data =
        material.texture.data;
}


// ============================================================
// Load Texture File
// ============================================================

static bool LoadTextureFile(
    TW::ModelMaterial& material,
    const std::string& filename
)
{
    if (filename.empty())
        return false;

    std::string fname = filename;

    Image image =
        LoadImage(fname.c_str());

    if (!image.data)
    {
        TraceLog(
            LOG_ERROR,
            "Could not load texture: %s",
            filename.c_str()
        );

        return false;
    }

    int dataSize = 0;

    unsigned char* pngData =
        ExportImageToMemory(
            image,
            ".png",
            &dataSize
        );

    UnloadImage(image);

    if (!pngData || dataSize <= 0)
    {
        TraceLog(
            LOG_ERROR,
            "Could not convert texture to PNG."
        );

        if (pngData)
            MemFree(pngData);

        return false;
    }

    std::vector<unsigned char> bytes(
        pngData,
        pngData + dataSize
    );

    MemFree(pngData);

    material.texture.embedded = true;
    material.texture.mime = "image/png";
    material.texture.data =
        TW::Base64Encode(bytes);

    return true;
}


// ============================================================
// Create Default Material
// ============================================================

static int CreateDefaultMaterial()
{
    TW::ModelMaterial material;

    material.name = "Material 0";

    material.texture.embedded = false;
    material.texture.mime = "";
    material.texture.data = "";

    model.materials.push_back(
        std::move(material)
    );

    materialPreviews.push_back({});

    return static_cast<int>(
        model.materials.size()
    ) - 1;
}


// ============================================================
// Create Primitive
// ============================================================

static void AddPrimitive(
    const std::string& type
)
{
    TW::ModelFrame frame;

    if (type == "Cube")
    {
        frame =
            TW::CreateCubeFrame();
    }
    else if (type == "Plane")
    {
        frame =
            TW::CreatePlaneFrame();
    }
    else if (type == "Sphere")
    {
        frame =
            TW::CreateSphereFrame();
    }
    else if (type == "Cylinder")
    {
        frame =
            TW::CreateCylinderFrame();
    }
    else if (type == "Cone")
    {
        frame =
            TW::CreateConeFrame();
    }
    else
    {
        return;
    }

    // --------------------------------------------------------
    // Make sure a material exists
    // --------------------------------------------------------

    if (model.materials.empty())
    {
        int materialIndex =
            CreateDefaultMaterial();

        for (TW::ModelSurface& surface :
             frame.surfaces)
        {
            surface.material =
                materialIndex;
        }
    }
    else
    {
        for (TW::ModelSurface& surface :
             frame.surfaces)
        {
            surface.material =
                selectedMaterial >= 0
                    ? selectedMaterial
                    : 0;
        }
    }

    model.frames.push_back(
        std::move(frame)
    );

    selectedFrame =
        static_cast<int>(
            model.frames.size()
        ) - 1;

    selectedSurface = -1;

    TraceLog(
        LOG_INFO,
        "Added %s.",
        type.c_str()
    );
}


// ============================================================
// Draw Model Frame
// ============================================================

static void DrawTWFrame(
    const TW::ModelFrame& frame
)
{
    if (frame.vertices.empty())
        return;

    if (frame.indices.empty())
        return;

    // --------------------------------------------------------
    // Draw each surface separately
    // --------------------------------------------------------

    for (const TW::ModelSurface& surface :
         frame.surfaces)
    {
        unsigned int start =
            surface.firstIndex;

        unsigned int count =
            surface.indexCount;

        if (start >= frame.indices.size())
            continue;

        if (start + count > frame.indices.size())
        {
            count =
                static_cast<unsigned int>(
                    frame.indices.size() - start
                );
        }

        int materialIndex =
            surface.material;

        Texture2D texture = {};

        bool hasTexture = false;

        if (materialIndex >= 0 &&
            materialIndex <
                static_cast<int>(model.materials.size()))
        {
            LoadMaterialPreview(
                materialIndex
            );

            if (materialIndex <
                static_cast<int>(materialPreviews.size()))
            {
                MaterialPreview& preview =
                    materialPreviews[materialIndex];

                if (preview.loaded)
                {
                    texture =
                        preview.texture;

                    hasTexture = true;
                }
            }
        }

        // ----------------------------------------------------
        // Bind texture
        // ----------------------------------------------------

        // if (hasTexture)
        //     rlSetTexture(texture.id);
        // else
        //     rlSetTexture(0);

        if (hasTexture)
        {
            rlEnableTexture(texture.id);
        }
        else
        {
            rlDisableTexture();
        }

        rlBegin(RL_TRIANGLES);

        for (unsigned int i = 0;
             i < count;
             i++)
        {
            unsigned int index =
                frame.indices[start + i];

            if (index >= frame.vertices.size())
                continue;

            const TW::ModelVertex& vertex =
                frame.vertices[index];

            // ------------------------------------------------
            // Normal
            // ------------------------------------------------

            rlNormal3f(
                vertex.normal.x,
                vertex.normal.y,
                vertex.normal.z
            );

            // ------------------------------------------------
            // Color
            // ------------------------------------------------

            rlColor4ub(
                vertex.color.r,
                vertex.color.g,
                vertex.color.b,
                vertex.color.a
            );

            // ------------------------------------------------
            // Texture coordinates
            // ------------------------------------------------

            if (hasTexture)
            {
                rlTexCoord2f(
                    vertex.texcoord.x,
                    vertex.texcoord.y
                );
            }

            // ------------------------------------------------
            // Position
            // ------------------------------------------------

            rlVertex3f(
                vertex.position.x,
                vertex.position.y,
                vertex.position.z
            );
        }

        rlEnd();

        rlDisableTexture();
        // rlSetTexture(0);
    }
}


// ============================================================
// Create Cube Shortcut
// ============================================================

static void CreateCube()
{
    AddPrimitive("Cube");
}


// ============================================================
// Main
// ============================================================

int main()
{
    // --------------------------------------------------------
    // Raylib
    // --------------------------------------------------------

    SetConfigFlags(
        FLAG_WINDOW_RESIZABLE |
        FLAG_MSAA_4X_HINT
    );

    InitWindow(
        1280,
        720,
        "TimWorks Model Maker"
    );

    SetTargetFPS(60);

    // --------------------------------------------------------
    // ImGui
    // --------------------------------------------------------

    rlImGuiSetup(true);

    // --------------------------------------------------------
    // Camera
    // --------------------------------------------------------

    Camera3D camera = {};

    camera.position =
        { 4.0f, 3.0f, 6.0f };

    camera.target =
        { 0.0f, 0.0f, 0.0f };

    camera.up =
        { 0.0f, 1.0f, 0.0f };

    camera.fovy = 45.0f;

    camera.projection =
        CAMERA_PERSPECTIVE;

    // --------------------------------------------------------
    // New model
    // --------------------------------------------------------

    model = TW::TWModel();

    // Create an initial material

    CreateDefaultMaterial();

    // Create an initial cube

    CreateCube();

    // --------------------------------------------------------
    // Main loop
    // --------------------------------------------------------

    while (!WindowShouldClose())
    {
        // ====================================================
        // Camera
        // ====================================================

        UpdateCamera(
            &camera,
            CAMERA_FREE
        );

        // ====================================================
        // Begin Drawing
        // ====================================================

        BeginDrawing();

        ClearBackground(
            Color{
                35,
                35,
                40,
                255
            }
        );

        // ====================================================
        // 3D Viewport
        // ====================================================

        BeginMode3D(camera);

        DrawGrid(
            20,
            1.0f
        );

        if (selectedFrame >= 0 &&
            selectedFrame <
                static_cast<int>(model.frames.size()))
        {
            DrawTWFrame(
                model.frames[selectedFrame]
            );
        }
        else
        {
            DrawTWFrame(
                model.idleFrame
            );
        }

        EndMode3D();

        // ====================================================
        // ImGui
        // ====================================================

        rlImGuiBegin();

        // ====================================================
        // Main Menu
        // ====================================================

        if (ImGui::BeginMainMenuBar())
        {
            // =================================================
            // File
            // =================================================

            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New"))
                {
                    for (int i = 0;
                         i < static_cast<int>(
                             materialPreviews.size());
                         i++)
                    {
                        UnloadMaterialPreview(i);
                    }

                    model = TW::TWModel();

                    currentFile.clear();

                    selectedMaterial = -1;
                    selectedFrame = 0;
                    selectedSurface = -1;
                    selectedAnimation = -1;

                    materialPreviews.clear();

                    CreateDefaultMaterial();

                    TraceLog(
                        LOG_INFO,
                        "Created new model."
                    );
                }

                if (ImGui::MenuItem("Open"))
                {
                    std::string filename =
                        OpenMDLDialog();

                    if (!filename.empty())
                    {
                        for (int i = 0;
                             i < static_cast<int>(
                                 materialPreviews.size());
                             i++)
                        {
                            UnloadMaterialPreview(i);
                        }

                        materialPreviews.clear();

                        TW::TWModel loadedModel;

                        if (TW::LoadTWModel(
                            loadedModel,
                            filename
                        ))
                        {
                            model =
                                std::move(
                                    loadedModel
                                );

                            currentFile =
                                filename;

                            selectedMaterial = -1;
                            selectedFrame = 0;
                            selectedSurface = -1;
                            selectedAnimation = -1;

                            materialPreviews.resize(
                                model.materials.size()
                            );

                            for (int i = 0;
                                 i < static_cast<int>(
                                     model.materials.size());
                                 i++)
                            {
                                UpdateMaterialPreview(i);
                            }

                            TraceLog(
                                LOG_INFO,
                                "Opened model: %s",
                                filename.c_str()
                            );
                        }
                        else
                        {
                            errorMessage =
                                "Could not open model:\n" +
                                filename;
                        }
                    }
                }

                if (ImGui::MenuItem(
                    "Save",
                    nullptr,
                    false,
                    !currentFile.empty()
                ))
                {
                    if (!TW::SaveTWModel(
                        model,
                        currentFile
                    ))
                    {
                        errorMessage =
                            "Could not save model.";
                    }
                }

                if (ImGui::MenuItem("Save As..."))
                {
                    std::string filename =
                        SaveMDLDialog();

                    if (!filename.empty())
                    {
                        if (TW::SaveTWModel(
                            model,
                            filename
                        ))
                        {
                            currentFile =
                                filename;
                        }
                        else
                        {
                            errorMessage =
                                "Could not save model.";
                        }
                    }
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit"))
                {
                    CloseWindow();

                    rlImGuiShutdown();

                    return 0;
                }

                ImGui::EndMenu();
            }

            // =================================================
            // Model
            // =================================================

            if (ImGui::BeginMenu("Model"))
            {
                if (ImGui::BeginMenu("Add Primitive"))
                {
                    if (ImGui::MenuItem("Cube"))
                    {
                        AddPrimitive("Cube");
                    }

                    if (ImGui::MenuItem("Plane"))
                    {
                        AddPrimitive("Plane");
                    }

                    if (ImGui::MenuItem("Sphere"))
                    {
                        AddPrimitive("Sphere");
                    }

                    if (ImGui::MenuItem("Cylinder"))
                    {
                        AddPrimitive("Cylinder");
                    }

                    if (ImGui::MenuItem("Cone"))
                    {
                        AddPrimitive("Cone");
                    }

                    ImGui::EndMenu();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Add Material"))
                {
                    int index =
                        CreateDefaultMaterial();

                    selectedMaterial =
                        index;
                }

                if (ImGui::MenuItem("Add Frame"))
                {
                    model.frames.push_back(
                        TW::ModelFrame()
                    );

                    selectedFrame =
                        static_cast<int>(
                            model.frames.size()
                        ) - 1;

                    selectedSurface = -1;
                }

                ImGui::EndMenu();
            }

            // =================================================
            // Tools
            // =================================================

            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem(
                    "Texture Maker"
                ))
                {
                    textureMaker.SetOpen(true);
                }

                ImGui::EndMenu();
            }

            // =================================================
            // Debug
            // =================================================

            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::MenuItem(
                    "ImGui Demo",
                    nullptr,
                    &showDemoWindow
                );

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }


        // ====================================================
        // Left Model Panel
        // ====================================================

        ImGui::SetNextWindowPos(
            ImVec2(10, 35),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(300, 650),
            ImGuiCond_Always
        );

        ImGui::Begin(
            "Model",
            nullptr,
            ImGuiWindowFlags_NoCollapse
        );

        // ----------------------------------------------------
        // Materials
        // ----------------------------------------------------

        if (ImGui::CollapsingHeader(
            "Materials",
            ImGuiTreeNodeFlags_DefaultOpen
        ))
        {
            if (ImGui::Button(
                "Add Material"
            ))
            {
                selectedMaterial =
                    CreateDefaultMaterial();
            }

            ImGui::Separator();

            for (int i = 0;
                 i < static_cast<int>(
                     model.materials.size());
                 i++)
            {
                TW::ModelMaterial& material =
                    model.materials[i];

                std::string label =
                    material.name;

                if (material.texture.embedded)
                    label += " [Texture]";

                if (ImGui::Selectable(
                    label.c_str(),
                    selectedMaterial == i
                ))
                {
                    selectedMaterial = i;

                    selectedSurface = -1;
                }
            }
        }


        // ----------------------------------------------------
        // Frames
        // ----------------------------------------------------

        if (ImGui::CollapsingHeader(
            "Frames",
            ImGuiTreeNodeFlags_DefaultOpen
        ))
        {
            for (int i = 0;
                 i < static_cast<int>(
                     model.frames.size());
                 i++)
            {
                std::string label =
                    "Frame " +
                    std::to_string(i);

                if (ImGui::Selectable(
                    label.c_str(),
                    selectedFrame == i
                ))
                {
                    selectedFrame = i;

                    selectedSurface = -1;
                }
            }

            if (model.frames.empty())
            {
                ImGui::TextDisabled(
                    "No frames."
                );
            }
        }


        // ----------------------------------------------------
        // Surfaces
        // ----------------------------------------------------

        if (ImGui::CollapsingHeader(
            "Surfaces",
            ImGuiTreeNodeFlags_DefaultOpen
        ))
        {
            if (selectedFrame >= 0 &&
                selectedFrame <
                    static_cast<int>(
                        model.frames.size()))
            {
                TW::ModelFrame& frame =
                    model.frames[selectedFrame];

                for (int i = 0;
                     i < static_cast<int>(
                         frame.surfaces.size());
                     i++)
                {
                    TW::ModelSurface& surface =
                        frame.surfaces[i];

                    std::string label =
                        "Surface " +
                        std::to_string(i);

                    if (surface.material >= 0 &&
                        surface.material <
                            static_cast<int>(
                                model.materials.size()))
                    {
                        label +=
                            " [" +
                            model.materials[
                                surface.material
                            ].name +
                            "]";
                    }
                    else
                    {
                        label +=
                            " [No Material]";
                    }

                    if (ImGui::Selectable(
                        label.c_str(),
                        selectedSurface == i
                    ))
                    {
                        selectedSurface = i;

                        selectedMaterial =
                            surface.material;
                    }
                }

                if (frame.surfaces.empty())
                {
                    ImGui::TextDisabled(
                        "No surfaces."
                    );
                }
            }
            else
            {
                ImGui::TextDisabled(
                    "Select a frame first."
                );
            }
        }


        // ----------------------------------------------------
        // Animations
        // ----------------------------------------------------

        if (ImGui::CollapsingHeader(
            "Animations"
        ))
        {
            for (int i = 0;
                 i < static_cast<int>(
                     model.animations.size());
                 i++)
            {
                if (ImGui::Selectable(
                    model.animations[i]
                        .name.c_str(),
                    selectedAnimation == i
                ))
                {
                    selectedAnimation = i;
                }
            }

            if (ImGui::Button(
                "Add Animation"
            ))
            {
                TW::ModelAnimation animation;

                animation.name =
                    "Animation " +
                    std::to_string(
                        model.animations.size()
                    );

                model.animations.push_back(
                    std::move(animation)
                );

                selectedAnimation =
                    static_cast<int>(
                        model.animations.size()
                    ) - 1;
            }
        }


        // ----------------------------------------------------
        // Statistics
        // ----------------------------------------------------

        if (ImGui::CollapsingHeader(
            "Statistics"
        ))
        {
            size_t vertices = 0;
            size_t indices = 0;
            size_t surfaces = 0;

            for (const TW::ModelFrame& frame :
                 model.frames)
            {
                vertices +=
                    frame.vertices.size();

                indices +=
                    frame.indices.size();

                surfaces +=
                    frame.surfaces.size();
            }

            ImGui::Text(
                "Materials: %d",
                static_cast<int>(
                    model.materials.size()
                )
            );

            ImGui::Text(
                "Frames: %d",
                static_cast<int>(
                    model.frames.size()
                )
            );

            ImGui::Text(
                "Vertices: %zu",
                vertices
            );

            ImGui::Text(
                "Indices: %zu",
                indices
            );

            ImGui::Text(
                "Surfaces: %zu",
                surfaces
            );

            ImGui::Text(
                "Animations: %d",
                static_cast<int>(
                    model.animations.size()
                )
            );
        }

        ImGui::End();


        // ====================================================
        // Right Inspector
        // ====================================================

        ImGui::SetNextWindowPos(
            ImVec2(
                static_cast<float>(
                    GetScreenWidth() - 330
                ),
                35
            ),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(320, 650),
            ImGuiCond_Always
        );

        ImGui::Begin(
            "Inspector",
            nullptr,
            ImGuiWindowFlags_NoCollapse
        );


        // ====================================================
        // Surface Inspector
        // ====================================================

        if (selectedFrame >= 0 &&
            selectedFrame <
                static_cast<int>(
                    model.frames.size()) &&
            selectedSurface >= 0 &&
            selectedSurface <
                static_cast<int>(
                    model.frames[
                        selectedFrame
                    ].surfaces.size()))
        {
            TW::ModelSurface& surface =
                model.frames[
                    selectedFrame
                ].surfaces[
                    selectedSurface
                ];

            ImGui::Text(
                "Surface %d",
                selectedSurface
            );

            ImGui::Separator();

            // ------------------------------------------------
            // Material Combo
            // ------------------------------------------------

            const char* currentMaterial =
                "None";

            if (surface.material >= 0 &&
                surface.material <
                    static_cast<int>(
                        model.materials.size()))
            {
                currentMaterial =
                    model.materials[
                        surface.material
                    ].name.c_str();
            }

            if (ImGui::BeginCombo(
                "Material",
                currentMaterial
            ))
            {
                if (ImGui::Selectable(
                    "None",
                    surface.material == -1
                ))
                {
                    surface.material = -1;
                    selectedMaterial = -1;
                }

                for (int i = 0;
                     i < static_cast<int>(
                         model.materials.size());
                     i++)
                {
                    bool isSelected =
                        surface.material == i;

                    if (ImGui::Selectable(
                        model.materials[i]
                            .name.c_str(),
                        isSelected
                    ))
                    {
                        surface.material = i;

                        selectedMaterial = i;

                        UpdateMaterialPreview(i);
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            // ------------------------------------------------
            // Shading
            // ------------------------------------------------

            const char* shadingName =
                surface.shading ==
                    TW::SurfaceShading::Flat
                    ? "Flat"
                    : "Smooth";

            if (ImGui::BeginCombo(
                "Shading",
                shadingName
            ))
            {
                if (ImGui::Selectable(
                    "Flat",
                    surface.shading ==
                        TW::SurfaceShading::Flat
                ))
                {
                    surface.shading =
                        TW::SurfaceShading::Flat;
                }

                if (ImGui::Selectable(
                    "Smooth",
                    surface.shading ==
                        TW::SurfaceShading::Smooth
                ))
                {
                    surface.shading =
                        TW::SurfaceShading::Smooth;
                }

                ImGui::EndCombo();
            }

            ImGui::Separator();
        }


        // ====================================================
        // Material Inspector
        // ====================================================

        if (selectedMaterial >= 0 &&
            selectedMaterial <
                static_cast<int>(
                    model.materials.size()))
        {
            TW::ModelMaterial& material =
                model.materials[
                    selectedMaterial
                ];

            ImGui::Text(
                "Material"
            );

            ImGui::Separator();

            // ------------------------------------------------
            // Material Name
            // ------------------------------------------------

            char nameBuffer[256] = {};

            std::snprintf(
                nameBuffer,
                sizeof(nameBuffer),
                "%s",
                material.name.c_str()
            );

            if (ImGui::InputText(
                "Name",
                nameBuffer,
                sizeof(nameBuffer)
            ))
            {
                material.name =
                    nameBuffer;
            }

            ImGui::Separator();

            // ------------------------------------------------
            // Texture
            // ------------------------------------------------

            ImGui::Text(
                "Texture"
            );

            if (material.texture.embedded &&
                !material.texture.data.empty())
            {
                if (selectedMaterial >=
                    static_cast<int>(
                        materialPreviews.size()))
                {
                    materialPreviews.resize(
                        model.materials.size()
                    );
                }

                MaterialPreview& preview =
                    materialPreviews[
                        selectedMaterial
                    ];

                if (!preview.loaded)
                {
                    UpdateMaterialPreview(
                        selectedMaterial
                    );
                }

                if (preview.loaded)
                {
                    float previewSize = 220.0f;

                    ImGui::Image(
                        (ImTextureID)(uintptr_t)preview.texture.id,
                        ImVec2(
                            previewSize,
                            previewSize
                        )
                    );
                }
                else
                {
                    ImGui::TextDisabled(
                        "Texture could not be loaded."
                    );
                }
            }
            else
            {
                ImGui::TextDisabled(
                    "No texture."
                );
            }

            // ------------------------------------------------
            // Load Texture
            // ------------------------------------------------

            if (ImGui::Button(
                "Load Texture"
            ))
            {
                std::string filename =
                    OpenTextureDialog();

                if (!filename.empty())
                {
                    if (LoadTextureFile(
                        material,
                        filename
                    ))
                    {
                        UpdateMaterialPreview(
                            selectedMaterial
                        );
                    }
                }
            }

            ImGui::SameLine();

            // ------------------------------------------------
            // Edit Texture
            // ------------------------------------------------

            if (ImGui::Button(
                "Edit Texture"
            ))
            {
                textureMaker.SetOpen(true);
            }

            // ------------------------------------------------
            // Remove Texture
            // ------------------------------------------------

            if (material.texture.embedded)
            {
                if (ImGui::Button(
                    "Remove Texture"
                ))
                {
                    UnloadMaterialPreview(
                        selectedMaterial
                    );

                    material.texture.embedded =
                        false;

                    material.texture.mime.clear();
                    material.texture.data.clear();
                }
            }

            // ------------------------------------------------
            // Apply Material To All Surfaces
            // ------------------------------------------------

            ImGui::Separator();

            if (ImGui::Button(
                "Apply to All Surfaces"
            ))
            {
                for (TW::ModelFrame& frame :
                     model.frames)
                {
                    for (TW::ModelSurface& surface :
                         frame.surfaces)
                    {
                        surface.material =
                            selectedMaterial;
                    }
                }
            }

            // ------------------------------------------------
            // Material Usage
            // ------------------------------------------------

            if (ImGui::CollapsingHeader(
                "Used By"
            ))
            {
                bool found = false;

                for (int f = 0;
                     f < static_cast<int>(
                         model.frames.size());
                     f++)
                {
                    const TW::ModelFrame& frame =
                        model.frames[f];

                    for (int s = 0;
                         s < static_cast<int>(
                             frame.surfaces.size());
                         s++)
                    {
                        if (frame.surfaces[s]
                                .material ==
                            selectedMaterial)
                        {
                            ImGui::Text(
                                "Frame %d / Surface %d",
                                f,
                                s
                            );

                            found = true;
                        }
                    }
                }

                if (!found)
                {
                    ImGui::TextDisabled(
                        "Not assigned."
                    );
                }
            }

            // ------------------------------------------------
            // Delete Material
            // ------------------------------------------------

            ImGui::Separator();

            if (ImGui::Button(
                "Delete Material"
            ))
            {
                int deleting =
                    selectedMaterial;

                UnloadMaterialPreview(
                    deleting
                );

                // Remove material

                model.materials.erase(
                    model.materials.begin()
                    + deleting
                );

                if (deleting <
                    static_cast<int>(
                        materialPreviews.size()))
                {
                    materialPreviews.erase(
                        materialPreviews.begin()
                        + deleting
                    );
                }

                // Fix surface material indices

                for (TW::ModelFrame& frame :
                     model.frames)
                {
                    for (TW::ModelSurface& surface :
                         frame.surfaces)
                    {
                        if (surface.material ==
                            deleting)
                        {
                            surface.material =
                                -1;
                        }
                        else if (
                            surface.material >
                            deleting)
                        {
                            surface.material--;
                        }
                    }
                }

                selectedMaterial = -1;
            }
        }


        // ====================================================
        // No Selection
        // ====================================================

        if (selectedMaterial < 0 &&
            selectedSurface < 0)
        {
            ImGui::TextDisabled(
                "Select a material or surface."
            );
        }

        ImGui::End();


        // ====================================================
        // Texture Maker
        // ====================================================

        textureMaker.Draw();

        // ----------------------------------------------------
        // Apply Texture Maker Texture
        // ----------------------------------------------------

        if (textureMaker.ConsumeApplyRequest())
        {
            if (selectedMaterial >= 0 &&
                selectedMaterial <
                    static_cast<int>(
                        model.materials.size()))
            {
                TW::ModelMaterial& material =
                    model.materials[
                        selectedMaterial
                    ];

                if (textureMaker.ApplyToMaterial(
                    material
                ))
                {
                    UpdateMaterialPreview(
                        selectedMaterial
                    );

                    TraceLog(
                        LOG_INFO,
                        "Texture applied to material: %s",
                        material.name.c_str()
                    );
                }
                else
                {
                    errorMessage =
                        "Could not apply texture "
                        "to material.";
                }
            }
            else
            {
                errorMessage =
                    "Select a material first.";
            }
        }


        // ====================================================
        // Error Popup
        // ====================================================

        if (!errorMessage.empty())
        {
            ImGui::OpenPopup(
                "Error"
            );
        }

        if (ImGui::BeginPopupModal(
            "Error",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize
        ))
        {
            ImGui::TextWrapped(
                "%s",
                errorMessage.c_str()
            );

            ImGui::Separator();

            if (ImGui::Button(
                "OK",
                ImVec2(120, 0)
            ))
            {
                errorMessage.clear();

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }


        // ====================================================
        // ImGui Demo
        // ====================================================

        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow(
                &showDemoWindow
            );
        }


        // ====================================================
        // End ImGui
        // ====================================================

        rlImGuiEnd();

        // ====================================================
        // End Drawing
        // ====================================================

        EndDrawing();
    }


    // ========================================================
    // Cleanup
    // ========================================================

    for (int i = 0;
         i < static_cast<int>(
             materialPreviews.size());
         i++)
    {
        UnloadMaterialPreview(i);
    }

    materialPreviews.clear();

    rlImGuiShutdown();

    CloseWindow();

    return 0;
}