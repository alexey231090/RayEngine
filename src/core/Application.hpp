#pragma once

#include <memory>
#include <string>
#include <vector>
#include "raylib.h"
#include "scene/Scene.hpp"
#include "renderer/RenderSystem.hpp"
#include "editor/EditorLayer.hpp"
#include "Layer.hpp"

namespace REngine {

struct AppConfig {
    int width = 1280;
    int height = 720;
    std::string title = "REngine — AI-First 3D Engine & ImGui Editor";
    int targetFPS = 60;
    int testFrames = -1; // -1 for infinite interactive loop, > 0 for automated exit after N frames
    bool headless = false; // Run with hidden window for automated testing
    std::string scenePath = "scene.json"; // Scene file to load
    bool isGameMode = false; // Run standalone game window without Editor UI (Godot style)
};

class Application {
public:
    Application(const AppConfig& config = AppConfig());
    ~Application();

    void Run();

    Scene& GetScene() { return m_scene; }
    void ClearScene(bool keepPrimaryCamera = true);

    // Layer stack operations for game modularity
    void PushLayer(std::shared_ptr<Layer> layer);

    template<typename T, typename... Args>
    std::shared_ptr<T> PushLayer(Args&&... args) {
        auto layer = std::make_shared<T>(std::forward<Args>(args)...);
        PushLayer(layer);
        return layer;
    }

    void PopLayer(std::shared_ptr<Layer> layer);

private:
    void InitDemoScene();
    void HandleCameraInput();
    Camera3D GetCurrentGameCamera();

private:
    AppConfig m_config;
    Scene m_scene;
    RenderSystem m_renderSystem;
    std::unique_ptr<EditorLayer> m_editorLayer;
    std::vector<std::shared_ptr<Layer>> m_layers;

    Camera3D m_editorCamera;
    std::string m_sceneSnapshot;
};

} // namespace REngine
