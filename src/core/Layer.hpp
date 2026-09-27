#pragma once

#include <string>
#include "raylib.h"

namespace REngine {

class Application;
class Scene;

class Layer {
public:
    Layer(const std::string& name = "Layer") : m_debugName(name) {}
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(float dt) {}
    virtual void OnRender3D() {}
    virtual void OnRenderUI() {}

    const std::string& GetName() const { return m_debugName; }

    // Direct engine context accessors for AI developers (no manual constructor wiring required)
    Application& GetApp();
    Scene& GetScene();
    Camera3D GetPrimaryCamera();
    void SetPrimaryCamera(const Camera3D& camera);

    // Instant HUD helpers for clean arcade typography
    void DrawHUDScoreboard(const char* p1Name, int p1Score, const char* p2Name, int p2Score, Color color = WHITE);
    void DrawCenterPrompt(const char* message, int fontSize = 28, Color color = YELLOW);

protected:
    std::string m_debugName;
};

} // namespace REngine
