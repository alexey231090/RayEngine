#pragma once

#include "raylib.h"
#include "scene/Scene.hpp"

namespace REngine {

class RenderSystem {
public:
    RenderSystem() = default;
    ~RenderSystem() = default;

    void Init();
    void Shutdown();

    void Render(Scene& scene, const Camera3D& camera, bool isEditMode = true, entt::entity selectedEntity = entt::null);

    void SetLightingEnabled(bool enabled) { m_lightingEnabled = enabled; }
    bool IsLightingEnabled() const { return m_lightingEnabled; }
    void SetLightDirection(Vector3 dir);

private:
    void DrawCameraGizmo(const Camera3D& gameCamera, bool isSelected = false);

private:
    Shader m_lightingShader = { 0 };
    bool m_shaderLoaded = false;
    bool m_lightingEnabled = true;

    int m_lightDirLoc = -1;
    int m_lightColorLoc = -1;
    int m_ambientColorLoc = -1;
    int m_viewPosLoc = -1;
};

} // namespace REngine
