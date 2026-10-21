#include "TextureMaker.h"

#include "../Base64/Base64.h"

#include <ImGUI/imgui.h>

#include <algorithm>
#include <cmath>
#include <cstdio>


namespace TW
{

TextureMaker::TextureMaker()
{
}


TextureMaker::~TextureMaker()
{
    if (textureValid)
    {
        UnloadTexture(texture);

        texture = {};
        textureValid = false;
    }

    if (image.data)
    {
        UnloadImage(image);

        image = {};
    }

    for (Image& undoImage : undoStack)
    {
        if (undoImage.data)
            UnloadImage(undoImage);
    }

    for (Image& redoImage : redoStack)
    {
        if (redoImage.data)
            UnloadImage(redoImage);
    }

    undoStack.clear();
    redoStack.clear();
}


void TextureMaker::SetOpen(bool value)
{
    open = value;
}


bool TextureMaker::IsOpen() const
{
    return open;
}


bool TextureMaker::HasTexture() const
{
    return image.data != nullptr;
}


int TextureMaker::GetWidth() const
{
    return image.width;
}


int TextureMaker::GetHeight() const
{
    return image.height;
}


const Image& TextureMaker::GetImage() const
{
    return image;
}


TextureTool TextureMaker::GetTool() const
{
    return tool;
}


void TextureMaker::SetTool(
    TextureTool newTool
)
{
    tool = newTool;
}


Color TextureMaker::GetColor() const
{
    return color;
}


void TextureMaker::SetColor(
    Color newColor
)
{
    color = newColor;
}


void TextureMaker::New(
    int width,
    int height
)
{
    if (width <= 0 ||
        height <= 0)
        return;

    if (textureValid)
    {
        UnloadTexture(texture);

        texture = {};
        textureValid = false;
    }

    if (image.data)
    {
        UnloadImage(image);

        image = {};
    }

    for (Image& undoImage : undoStack)
    {
        if (undoImage.data)
            UnloadImage(undoImage);
    }

    for (Image& redoImage : redoStack)
    {
        if (redoImage.data)
            UnloadImage(redoImage);
    }

    undoStack.clear();
    redoStack.clear();

    image = GenImageColor(
        width,
        height,
        BLANK
    );

    zoom = 16.0f;

    ResetPan();

    UpdateTexture();
}


void TextureMaker::ResetPan()
{
    pan = {
        0.0f,
        0.0f
    };
}


void TextureMaker::UpdateTexture()
{
    if (!image.data)
        return;

    if (textureValid)
    {
        UnloadTexture(texture);

        texture = {};
        textureValid = false;
    }

    texture =
        LoadTextureFromImage(image);

    SetTextureFilter(
        texture,
        TEXTURE_FILTER_POINT
    );

    textureValid =
        IsTextureValid(texture);
}


void TextureMaker::PushUndo()
{
    if (!image.data)
        return;

    Image copy =
        ImageCopy(image);

    undoStack.push_back(copy);

    if (undoStack.size() >
        static_cast<size_t>(maxUndo))
    {
        UnloadImage(
            undoStack.front()
        );

        undoStack.erase(
            undoStack.begin()
        );
    }

    for (Image& redoImage : redoStack)
    {
        if (redoImage.data)
            UnloadImage(redoImage);
    }

    redoStack.clear();
}


void TextureMaker::Undo()
{
    if (undoStack.empty())
        return;

    if (image.data)
    {
        redoStack.push_back(
            ImageCopy(image)
        );

        UnloadImage(image);
    }

    image =
        undoStack.back();

    undoStack.pop_back();

    UpdateTexture();
}


void TextureMaker::Redo()
{
    if (redoStack.empty())
        return;

    if (image.data)
    {
        undoStack.push_back(
            ImageCopy(image)
        );

        UnloadImage(image);
    }

    image =
        redoStack.back();

    redoStack.pop_back();

    UpdateTexture();
}


void TextureMaker::DrawPixel(
    int x,
    int y,
    Color drawColor
)
{
    if (!image.data)
        return;

    if (x < 0 ||
        y < 0 ||
        x >= image.width ||
        y >= image.height)
        return;

    ImageDrawPixel(
        &image,
        x,
        y,
        drawColor
    );
}


void TextureMaker::DrawBrush(
    int x,
    int y,
    Color drawColor
)
{
    int radius =
        brushSize / 2;

    for (int yy = -radius;
         yy <= radius;
         yy++)
    {
        for (int xx = -radius;
             xx <= radius;
             xx++)
        {
            DrawPixel(
                x + xx,
                y + yy,
                drawColor
            );
        }
    }
}


void TextureMaker::Fill(
    int x,
    int y,
    Color fillColor
)
{
    if (!image.data)
        return;

    if (x < 0 ||
        y < 0 ||
        x >= image.width ||
        y >= image.height)
        return;

    Color* pixels =
        static_cast<Color*>(image.data);

    Color target =
        pixels[
            y * image.width + x
        ];

    if (target.r == fillColor.r &&
        target.g == fillColor.g &&
        target.b == fillColor.b &&
        target.a == fillColor.a)
    {
        return;
    }

    std::vector<Vector2> stack;

    stack.push_back({
        static_cast<float>(x),
        static_cast<float>(y)
    });

    while (!stack.empty())
    {
        Vector2 current =
            stack.back();

        stack.pop_back();

        int px =
            static_cast<int>(current.x);

        int py =
            static_cast<int>(current.y);

        if (px < 0 ||
            py < 0 ||
            px >= image.width ||
            py >= image.height)
            continue;

        Color& pixel =
            pixels[
                py * image.width + px
            ];

        if (pixel.r != target.r ||
            pixel.g != target.g ||
            pixel.b != target.b ||
            pixel.a != target.a)
        {
            continue;
        }

        pixel = fillColor;

        stack.push_back({
            static_cast<float>(px + 1),
            static_cast<float>(py)
        });

        stack.push_back({
            static_cast<float>(px - 1),
            static_cast<float>(py)
        });

        stack.push_back({
            static_cast<float>(px),
            static_cast<float>(py + 1)
        });

        stack.push_back({
            static_cast<float>(px),
            static_cast<float>(py - 1)
        });
    }
}


void TextureMaker::PaintAtMouse()
{
    if (!image.data)
        return;

    ImVec2 canvasStart =
        ImGui::GetCursorScreenPos();

    ImVec2 mouse =
        ImGui::GetMousePos();

    float localX =
        mouse.x -
        canvasStart.x -
        pan.x;

    float localY =
        mouse.y -
        canvasStart.y -
        pan.y;

    int pixelX =
        static_cast<int>(
            std::floor(localX / zoom)
        );

    int pixelY =
        static_cast<int>(
            std::floor(localY / zoom)
        );

    if (pixelX < 0 ||
        pixelY < 0 ||
        pixelX >= image.width ||
        pixelY >= image.height)
        return;

    if (tool ==
        TextureTool::Pencil)
    {
        DrawBrush(
            pixelX,
            pixelY,
            color
        );
    }
    else if (tool ==
             TextureTool::Eraser)
    {
        DrawBrush(
            pixelX,
            pixelY,
            BLANK
        );
    }
    else if (tool ==
             TextureTool::ColorPicker)
    {
        Color* pixels =
            static_cast<Color*>(
                image.data
            );

        color =
            pixels[
                pixelY * image.width +
                pixelX
            ];

        return;
    }
}


void TextureMaker::DrawCanvas()
{
    if (!textureValid)
        return;

    ImVec2 canvasStart =
        ImGui::GetCursorScreenPos();

    ImVec2 available =
        ImGui::GetContentRegionAvail();

    if (available.x < 32.0f)
        available.x = 32.0f;

    if (available.y < 32.0f)
        available.y = 32.0f;

    ImGui::InvisibleButton(
        "TextureCanvas",
        available,
        ImGuiButtonFlags_MouseButtonLeft |
        ImGuiButtonFlags_MouseButtonMiddle
    );

    bool hovered =
        ImGui::IsItemHovered();

    ImDrawList* drawList =
        ImGui::GetWindowDrawList();

    ImVec2 canvasEnd(
        canvasStart.x + available.x,
        canvasStart.y + available.y
    );

    drawList->AddRectFilled(
        canvasStart,
        canvasEnd,
        IM_COL32(
            45,
            45,
            45,
            255
        )
    );

    // ------------------------------------------------------------
    // Middle mouse panning
    // ------------------------------------------------------------

    if (hovered &&
        ImGui::IsMouseClicked(
            ImGuiMouseButton_Middle))
    {
        panning = true;

        panStart.x =
            ImGui::GetMousePos().x;

        panStart.y =
            ImGui::GetMousePos().y;

        panStartOffset =
            pan;
    }

    if (!ImGui::IsMouseDown(
        ImGuiMouseButton_Middle))
    {
        panning = false;
    }

    if (panning)
    {
        ImVec2 mouse =
            ImGui::GetMousePos();

        pan.x =
            panStartOffset.x +
            (mouse.x - panStart.x);

        pan.y =
            panStartOffset.y +
            (mouse.y - panStart.y);
    }

    // ------------------------------------------------------------
    // Mouse wheel zoom
    // ------------------------------------------------------------

    if (hovered)
    {
        float wheel =
            ImGui::GetIO().MouseWheel;

        if (wheel != 0.0f)
        {
            ImVec2 mouse =
                ImGui::GetMousePos();

            float oldZoom =
                zoom;

            if (wheel > 0.0f)
                zoom *= 1.25f;
            else
                zoom /= 1.25f;

            zoom =
                std::clamp(
                    zoom,
                    1.0f,
                    128.0f
                );

            float relativeX =
                mouse.x -
                canvasStart.x -
                pan.x;

            float relativeY =
                mouse.y -
                canvasStart.y -
                pan.y;

            float scale =
                zoom / oldZoom;

            pan.x =
                mouse.x -
                canvasStart.x -
                relativeX * scale;

            pan.y =
                mouse.y -
                canvasStart.y -
                relativeY * scale;
        }
    }

    // ------------------------------------------------------------
    // Texture position
    // ------------------------------------------------------------

    ImVec2 textureStart(
        canvasStart.x + pan.x,
        canvasStart.y + pan.y
    );

    ImVec2 textureEnd(
        textureStart.x +
            image.width * zoom,

        textureStart.y +
            image.height * zoom
    );

    // ------------------------------------------------------------
    // Texture
    // ------------------------------------------------------------

    Rectangle source = {
        0.0f,
        0.0f,
        static_cast<float>(image.width),
        static_cast<float>(image.height)
    };

    Rectangle destination = {
        textureStart.x,
        textureStart.y,
        image.width * zoom,
        image.height * zoom
    };

    drawList->AddImage(
        (ImTextureID)(intptr_t)texture.id,
        textureStart,
        textureEnd,
        ImVec2(0, 0),
        ImVec2(1, 1)
    );

    // ------------------------------------------------------------
    // Pixel grid
    // ------------------------------------------------------------

    if (zoom >= 4.0f)
    {
        for (int x = 0;
             x <= image.width;
             x++)
        {
            float px =
                textureStart.x +
                x * zoom;

            drawList->AddLine(
                ImVec2(
                    px,
                    textureStart.y
                ),
                ImVec2(
                    px,
                    textureEnd.y
                ),
                IM_COL32(
                    0,
                    0,
                    0,
                    80
                )
            );
        }

        for (int y = 0;
             y <= image.height;
             y++)
        {
            float py =
                textureStart.y +
                y * zoom;

            drawList->AddLine(
                ImVec2(
                    textureStart.x,
                    py
                ),
                ImVec2(
                    textureEnd.x,
                    py
                ),
                IM_COL32(
                    0,
                    0,
                    0,
                    80
                )
            );
        }
    }

    // ------------------------------------------------------------
    // Painting
    // ------------------------------------------------------------

    bool insideTexture =
        hovered &&
        ImGui::GetMousePos().x >=
            textureStart.x &&
        ImGui::GetMousePos().x <=
            textureEnd.x &&
        ImGui::GetMousePos().y >=
            textureStart.y &&
        ImGui::GetMousePos().y <=
            textureEnd.y;

    if (insideTexture &&
        ImGui::IsMouseClicked(
            ImGuiMouseButton_Left))
    {
        if (tool ==
                TextureTool::Fill)
        {
            ImVec2 mouse =
                ImGui::GetMousePos();

            int pixelX =
                static_cast<int>(
                    (mouse.x -
                     textureStart.x) /
                    zoom
                );

            int pixelY =
                static_cast<int>(
                    (mouse.y -
                     textureStart.y) /
                    zoom
                );

            PushUndo();

            Fill(
                pixelX,
                pixelY,
                color
            );

            UpdateTexture();
        }
        else
        {
            PushUndo();

            painting = true;

            PaintAtMouse();

            UpdateTexture();
        }
    }

    if (!ImGui::IsMouseDown(
        ImGuiMouseButton_Left))
    {
        painting = false;
    }

    if (insideTexture &&
        painting &&
        (tool ==
             TextureTool::Pencil ||
         tool ==
             TextureTool::Eraser))
    {
        PaintAtMouse();

        UpdateTexture();
    }
}


void TextureMaker::Draw()
{
    if (!open)
        return;

    ImGui::Begin(
        "Texture Maker",
        &open
    );

    if (!image.data)
    {
        ImGui::Text(
            "Create a texture to begin."
        );

        static int newWidth = 256;
        static int newHeight = 256;

        ImGui::InputInt(
            "Width",
            &newWidth
        );

        ImGui::InputInt(
            "Height",
            &newHeight
        );

        newWidth =
            std::max(newWidth, 1);

        newHeight =
            std::max(newHeight, 1);

        if (ImGui::Button(
            "Create Texture"
        ))
        {
            New(
                newWidth,
                newHeight
            );
        }

        ImGui::End();

        return;
    }

    // ------------------------------------------------------------
    // Toolbar
    // ------------------------------------------------------------

    if (ImGui::Button("Pencil"))
        tool = TextureTool::Pencil;

    ImGui::SameLine();

    if (ImGui::Button("Eraser"))
        tool = TextureTool::Eraser;

    ImGui::SameLine();

    if (ImGui::Button("Fill"))
        tool = TextureTool::Fill;

    ImGui::SameLine();

    if (ImGui::Button("Picker"))
        tool = TextureTool::ColorPicker;

    ImGui::Separator();

    // ------------------------------------------------------------
    // Color
    // ------------------------------------------------------------

    float colorValues[4] =
    {
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    };

    if (ImGui::ColorEdit4(
        "Color",
        colorValues
    ))
    {
        color.r =
            static_cast<unsigned char>(
                colorValues[0] * 255.0f
            );

        color.g =
            static_cast<unsigned char>(
                colorValues[1] * 255.0f
            );

        color.b =
            static_cast<unsigned char>(
                colorValues[2] * 255.0f
            );

        color.a =
            static_cast<unsigned char>(
                colorValues[3] * 255.0f
            );
    }

    ImGui::SameLine();

    ImGui::SetNextItemWidth(100);

    ImGui::SliderInt(
        "Brush",
        &brushSize,
        1,
        16
    );

    // ------------------------------------------------------------
    // Zoom controls
    // ------------------------------------------------------------

    ImGui::Separator();

    if (ImGui::Button("Zoom -"))
    {
        zoom /= 1.25f;

        zoom =
            std::clamp(
                zoom,
                1.0f,
                128.0f
            );
    }

    ImGui::SameLine();

    ImGui::Text(
        "Zoom: %.1fx",
        zoom
    );

    ImGui::SameLine();

    if (ImGui::Button("Zoom +"))
    {
        zoom *= 1.25f;

        zoom =
            std::clamp(
                zoom,
                1.0f,
                128.0f
            );
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset View"))
    {
        zoom = 16.0f;
        ResetPan();
    }

    ImGui::SameLine();

    if (ImGui::Button("Undo"))
        Undo();

    ImGui::SameLine();

    if (ImGui::Button("Redo"))
        Redo();

    ImGui::SameLine();

    if (ImGui::Button("Clear"))
    {
        PushUndo();

        ImageClearBackground(
            &image,
            BLANK
        );

        UpdateTexture();
    }

    // ------------------------------------------------------------
    // Apply
    // ------------------------------------------------------------

    ImGui::Separator();

    if (ImGui::Button(
        "Apply to Material"
    ))
    {
        applyRequest = true;
    }

    ImGui::SameLine();

    ImGui::Text(
        "%d x %d",
        image.width,
        image.height
    );

    ImGui::Separator();

    // ------------------------------------------------------------
    // Canvas
    // ------------------------------------------------------------

    DrawCanvas();

    ImGui::End();
}


Image TextureMaker::ExportImage() const
{
    if (!image.data)
        return {};

    return ImageCopy(image);
}


std::string TextureMaker::GetPNGBase64() const
{
    if (!image.data)
        return "";

    int dataSize = 0;

    unsigned char* pngData =
        ExportImageToMemory(
            image,
            ".png",
            &dataSize
        );

    if (pngData == nullptr || dataSize <= 0)
    {
        if (pngData)
            MemFree(pngData);

        TraceLog(
            LOG_ERROR,
            "TextureMaker: Failed to export PNG."
        );

        return "";
    }

    std::vector<unsigned char> bytes(
        pngData,
        pngData + dataSize
    );

    MemFree(pngData);

    std::string result =
        Base64Encode(bytes);

    if (result.empty())
    {
        TraceLog(
            LOG_ERROR,
            "TextureMaker: Base64 encoding failed."
        );
    }

    return result;
}


bool TextureMaker::ApplyToMaterial(
    ModelMaterial& material
) const
{
    if (!image.data)
        return false;

    std::string base64 =
        GetPNGBase64();

    if (base64.empty())
        return false;

    material.texture.embedded = true;

    material.texture.mime =
        "image/png";

    material.texture.data =
        base64;

    return true;
}


void TextureMaker::SavePNG(
    const std::string& filename
)
{
    if (!image.data)
        return;

    int dataSize = 0;

    unsigned char* pngData =
        ExportImageToMemory(
            image,
            ".png",
            &dataSize
        );

    if (!pngData ||
        dataSize <= 0)
    {
        return;
    }

    FILE* file =
        fopen(
            filename.c_str(),
            "wb"
        );

    if (file)
    {
        fwrite(
            pngData,
            1,
            dataSize,
            file
        );

        fclose(file);
    }

    MemFree(pngData);
}


bool TextureMaker::ConsumeApplyRequest()
{
    if (!applyRequest)
        return false;

    applyRequest = false;

    return true;
}

}