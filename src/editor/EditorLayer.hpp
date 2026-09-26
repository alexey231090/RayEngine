#pragma once

#include "raylib.h"
#include "scene/Scene.hpp"
#include "entt/entt.hpp"
#include "Gizmo.hpp"

namespace REngine {

enum class EngineMode {
    Edit,
    Play
};

class EditorLayer {
public:
    EditorLayer(Scene& scene);
    ~EditorLayer();

    void Init();
    void Shutdown();

    void BeginFrame();
    void RenderUI();
    void EndFrame();

    void RenderGizmo(const Camera3D& camera);

    entt::entity GetSelectedEntity() const { return m_selectedEntity; }
    void SetSelectedEntity(entt::entity entity) { m_selectedEntity = entity; }

    EngineMode GetEngineMode() const { return m_mode; }
    void SetEngineMode(EngineMode mode) { m_mode = mode; }
    bool HasModeChanged() {
        bool changed = m_modeChanged;
        m_modeChanged = false;
        return changed;
    }

    bool IsGizmoDragging() const { return m_gizmo.IsDragging(); }

private:
    void DrawToolbarPanel();
    void DrawHierarchyPanel();
    void DrawInspectorPanel();
    void DrawStatsPanel();

private:
    Scene& m_scene;
    entt::entity m_selectedEntity = entt::null;
    std::string m_sceneFilePath = "scene.json";
    std::string m_statusMessage = "Ready";

    EngineMode m_mode = EngineMode::Edit;
    bool m_modeChanged = false;

    Gizmo m_gizmo;
};

} // namespace REngine
