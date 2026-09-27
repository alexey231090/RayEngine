#include "Layer.hpp"
#include "Application.hpp"
#include "ui/EngineFont.hpp"

namespace REngine {

Application& Layer::GetApp() {
    return Application::Get();
}

Scene& Layer::GetScene() {
    return Application::Get().GetScene();
}

Camera3D Layer::GetPrimaryCamera() {
    return Application::Get().GetPrimaryCamera();
}

void Layer::SetPrimaryCamera(const Camera3D& camera) {
    Application::Get().SetPrimaryCamera(camera);
}

void Layer::DrawHUDScoreboard(const char* p1Name, int p1Score, const char* p2Name, int p2Score, Color color) {
    int screenW = GetScreenWidth();
    char textBuf[128];

    // Player 1 (Left)
    snprintf(textBuf, sizeof(textBuf), "%s: %d", p1Name, p1Score);
    EngineFont::DrawTextUTF8(textBuf, 30.0f, 25.0f, 26.0f, color);

    // Player 2 (Right)
    snprintf(textBuf, sizeof(textBuf), "%s: %d", p2Name, p2Score);
    Vector2 p2Size = EngineFont::MeasureTextUTF8(textBuf, 26.0f);
    EngineFont::DrawTextUTF8(textBuf, (float)screenW - p2Size.x - 30.0f, 25.0f, 26.0f, color);
}

void Layer::DrawCenterPrompt(const char* message, int fontSize, Color color) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    Vector2 textSize = EngineFont::MeasureTextUTF8(message, (float)fontSize);

    float posX = ((float)screenW - textSize.x) * 0.5f;
    float posY = ((float)screenH - textSize.y) * 0.5f;

    // Draw subtle dark background pill behind prompt for readability
    DrawRectangleRounded((Rectangle){ posX - 15.0f, posY - 8.0f, textSize.x + 30.0f, textSize.y + 16.0f }, 0.3f, 4, (Color){ 0, 0, 0, 180 });
    EngineFont::DrawTextUTF8(message, posX, posY, (float)fontSize, color);
}

} // namespace REngine
