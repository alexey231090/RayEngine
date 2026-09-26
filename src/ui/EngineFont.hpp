#pragma once

#include "raylib.h"

namespace REngine {

class EngineFont {
public:
    static void Init();
    static void Shutdown();

    static Font GetFont();
    static bool IsLoaded();

    static void DrawTextUTF8(const char* text, float posX, float posY, float fontSize, Color color);
    static void DrawTextUTF8Ex(const char* text, Vector2 position, float fontSize, float spacing, Color color);
    static Vector2 MeasureTextUTF8(const char* text, float fontSize, float spacing = 1.0f);

private:
    static Font s_font;
    static bool s_loaded;
};

inline void DrawTextUTF8(const char* text, float posX, float posY, float fontSize, Color color) {
    EngineFont::DrawTextUTF8(text, posX, posY, fontSize, color);
}

inline Vector2 MeasureTextUTF8(const char* text, float fontSize, float spacing = 1.0f) {
    return EngineFont::MeasureTextUTF8(text, fontSize, spacing);
}

} // namespace REngine
