#include "EngineFont.hpp"
#include <vector>
#include <iostream>

namespace REngine {

Font EngineFont::s_font = { 0 };
bool EngineFont::s_loaded = false;

void EngineFont::Init() {
    if (s_loaded) return;

    const char* fontPath = nullptr;
    if (FileExists("C:\\Windows\\Fonts\\segoeui.ttf")) fontPath = "C:\\Windows\\Fonts\\segoeui.ttf";
    else if (FileExists("C:\\Windows\\Fonts\\arial.ttf")) fontPath = "C:\\Windows\\Fonts\\arial.ttf";
    else if (FileExists("C:\\Windows\\Fonts\\tahoma.ttf")) fontPath = "C:\\Windows\\Fonts\\tahoma.ttf";

    if (fontPath) {
        // Collect codepoints for ASCII (32-126) + Latin-1 (160-255) + Cyrillic (0x0400-0x04FF)
        std::vector<int> codepoints;
        codepoints.reserve(512);

        for (int i = 32; i <= 126; ++i) codepoints.push_back(i);
        for (int i = 160; i <= 255; ++i) codepoints.push_back(i);
        for (int i = 0x0400; i <= 0x04FF; ++i) codepoints.push_back(i);

        // Load at 32px for crisp rendering and downscaling
        s_font = LoadFontEx(fontPath, 32, codepoints.data(), static_cast<int>(codepoints.size()));
        SetTextureFilter(s_font.texture, TEXTURE_FILTER_BILINEAR);
        s_loaded = true;
        std::cout << "[REngine] [Font] Loaded UTF-8 Cyrillic font from '" << fontPath << "'" << std::endl;
    } else {
        s_font = GetFontDefault();
        s_loaded = true;
        std::cout << "[REngine] [Font] Windows Cyrillic font not found; using Raylib default font" << std::endl;
    }
}

void EngineFont::Shutdown() {
    if (s_loaded && s_font.texture.id != GetFontDefault().texture.id) {
        UnloadFont(s_font);
        s_loaded = false;
    }
}

Font EngineFont::GetFont() {
    if (!s_loaded) Init();
    return s_font;
}

bool EngineFont::IsLoaded() {
    return s_loaded;
}

void EngineFont::DrawTextUTF8(const char* text, float posX, float posY, float fontSize, Color color) {
    if (!s_loaded) Init();
    Vector2 pos = { posX, posY };
    float spacing = 1.0f;
    DrawTextEx(s_font, text, pos, fontSize, spacing, color);
}

void EngineFont::DrawTextUTF8Ex(const char* text, Vector2 position, float fontSize, float spacing, Color color) {
    if (!s_loaded) Init();
    DrawTextEx(s_font, text, position, fontSize, spacing, color);
}

Vector2 EngineFont::MeasureTextUTF8(const char* text, float fontSize, float spacing) {
    if (!s_loaded) Init();
    return MeasureTextEx(s_font, text, fontSize, spacing);
}

} // namespace REngine
