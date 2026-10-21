#pragma once

#include <raylib/raylib.h>

#include <string>
#include <vector>

#include "../Model/TWModel.h"

namespace TW
{

enum class TextureTool
{
    Pencil,
    Eraser,
    Fill,
    ColorPicker
};

class TextureMaker
{
public:

    TextureMaker();
    ~TextureMaker();

    void New(
        int width,
        int height
    );

    void Draw();

    void SetOpen(bool value);
    bool IsOpen() const;

    bool HasTexture() const;

    int GetWidth() const;
    int GetHeight() const;

    const Image& GetImage() const;

    Image ExportImage() const;

    std::string GetPNGBase64() const;

    bool ApplyToMaterial(
        ModelMaterial& material
    ) const;

    void SavePNG(
        const std::string& filename
    );

    TextureTool GetTool() const;
    void SetTool(TextureTool tool);

    Color GetColor() const;
    void SetColor(Color color);

    bool ConsumeApplyRequest();

private:

    void UpdateTexture();

    void PushUndo();

    void Undo();
    void Redo();

    void DrawCanvas();

    void DrawPixel(
        int x,
        int y,
        Color color
    );

    void DrawBrush(
        int x,
        int y,
        Color color
    );

    void Fill(
        int x,
        int y,
        Color color
    );

    void PaintAtMouse();

    void ResetPan();

private:

    bool open = false;

    Image image = {};

    Texture2D texture = {};

    bool textureValid = false;

    TextureTool tool =
        TextureTool::Pencil;

    Color color = BLACK;

    int brushSize = 1;

    float zoom = 16.0f;

    Vector2 pan = {
        0.0f,
        0.0f
    };

    bool panning = false;

    Vector2 panStart = {
        0.0f,
        0.0f
    };

    Vector2 panStartOffset = {
        0.0f,
        0.0f
    };

    bool painting = false;

    bool applyRequest = false;

    std::vector<Image> undoStack;
    std::vector<Image> redoStack;

    int maxUndo = 32;
};

}