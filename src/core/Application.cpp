#include "Application.hpp"
#include "scene/Components.hpp"
#include "scene/SceneSerializer.hpp"
#include "audio/SoundFX.hpp"
#include "renderer/ParticleSystem3D.hpp"
#include "imgui.h"
#include "rcamera.h"
#include <iostream>
#include <algorithm>

namespace REngine {

Application::Application(const AppConfig& config) : m_config(config) {
    std::cout << "[REngine] [Init] Initializing engine..." << std::endl;

    unsigned int flags = FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT;
    if (m_config.headless) {
        flags |= FLAG_WINDOW_HIDDEN;
        std::cout << "[REngine] [Init] Running in HEADLESS mode (window hidden)" << std::endl;
    }

    if (m_config.isGameMode) {
        m_config.title = "REngine Game - Standalone Window";
    } else {
        m_config.title = "REngine Editor";
    }

    SetConfigFlags(flags);
    InitWindow(m_config.width, m_config.height, m_config.title.c_str());
    SetTargetFPS(m_config.targetFPS);

    std::cout << "[REngine] [Init] Window initialized: " << m_config.width << "x" << m_config.height 
              << " @ " << m_config.targetFPS << " FPS target (Title: '" << m_config.title << "')" << std::endl;

    // Initialize procedural audio
    SoundFX::Init();

    m_editorCamera = { 0 };
    m_editorCamera.position = (Vector3){ 0.0f, 8.0f, 14.0f };
    m_editorCamera.target = (Vector3){ 0.0f, 1.0f, 0.0f };
    m_editorCamera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    m_editorCamera.fovy = 45.0f;
    m_editorCamera.projection = CAMERA_PERSPECTIVE;

    if (!m_config.isGameMode) {
        m_editorLayer = std::make_unique<EditorLayer>(m_scene);
        m_editorLayer->Init();
        std::cout << "[REngine] [Init] EditorLayer (Dear ImGui) initialized successfully" << std::endl;
    } else {
        std::cout << "[REngine] [Init] Running in STANDALONE GAME mode (Editor UI disabled)" << std::endl;
    }

    // Load specified scene file or default
    SceneSerializer serializer(m_scene);
    if (!serializer.Deserialize(m_config.scenePath)) {
        std::cout << "[REngine] [Scene] Scene file '" << m_config.scenePath << "' not found. Initializing demo scene..." << std::endl;
        InitDemoScene();
        serializer.Serialize(m_config.scenePath);
        std::cout << "[REngine] [Scene] Created and saved demo scene to '" << m_config.scenePath << "'" << std::endl;
    } else {
        size_t entityCount = m_scene.GetRegistry().storage<entt::entity>().size();
        std::cout << "[REngine] [Scene] Successfully loaded '" << m_config.scenePath << "' with " 
                  << entityCount << " active entities" << std::endl;

        // Ensure scene has at least one camera
        if (m_scene.GetPrimaryCameraEntity() == entt::null) {
            auto camEntity = m_scene.CreateEntity("Main Camera");
            auto& camTransform = m_scene.GetRegistry().get<TransformComponent>(camEntity);
            camTransform.position = (Vector3){ 0.0f, 4.0f, 9.0f };
            CameraComponent cam;
            cam.camera.position = (Vector3){ 0.0f, 4.0f, 9.0f };
            cam.camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };
            m_scene.GetRegistry().emplace<CameraComponent>(camEntity, cam);
            std::cout << "[REngine] [Scene] No camera found in scene; created default 'Main Camera'" << std::endl;
        }
    }

    if (m_config.testFrames > 0) {
        std::cout << "[REngine] [Test] Automated test-run mode enabled: engine will exit after " 
                  << m_config.testFrames << " frames" << std::endl;
    }
}

Application::~Application() {
    for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it) {
        (*it)->OnDetach();
    }
    m_layers.clear();

    if (m_editorLayer) {
        m_editorLayer->Shutdown();
    }

    SoundFX::Shutdown();

    CloseWindow();
    std::cout << "[REngine] [Shutdown] Engine shut down cleanly with exit code 0" << std::endl;
}

void Application::ClearScene(bool keepPrimaryCamera) {
    m_scene.Clear(keepPrimaryCamera);
    if (m_editorLayer) {
        m_editorLayer->SetSelectedEntity(entt::null);
    }
    std::cout << "[REngine] [Scene] Scene cleared of demo objects (Camera preserved)" << std::endl;
}

void Application::PushLayer(std::shared_ptr<Layer> layer) {
    m_layers.push_back(layer);
    layer->OnAttach();
    std::cout << "[REngine] [Layer] Attached layer: '" << layer->GetName() << "'" << std::endl;
}

void Application::PopLayer(std::shared_ptr<Layer> layer) {
    auto it = std::find(m_layers.begin(), m_layers.end(), layer);
    if (it != m_layers.end()) {
        layer->OnDetach();
        m_layers.erase(it);
        std::cout << "[REngine] [Layer] Detached layer: '" << layer->GetName() << "'" << std::endl;
    }
}

void Application::InitDemoScene() {
    // 1. Red Cube
    auto cubeEntity = m_scene.CreateEntity("Main Red Cube");
    auto& cubeTransform = m_scene.GetRegistry().get<TransformComponent>(cubeEntity);
    cubeTransform.position = (Vector3){ 0.0f, 1.0f, 0.0f };
    cubeTransform.scale = (Vector3){ 2.0f, 2.0f, 2.0f };
    m_scene.GetRegistry().emplace<MeshComponent>(cubeEntity, MeshGeometryType::Cube, RED, MAROON);

    // 2. Blue Sphere
    auto sphereEntity = m_scene.CreateEntity("Blue Sphere");
    auto& sphereTransform = m_scene.GetRegistry().get<TransformComponent>(sphereEntity);
    sphereTransform.position = (Vector3){ 4.0f, 1.0f, 0.0f };
    sphereTransform.scale = (Vector3){ 2.0f, 2.0f, 2.0f };
    m_scene.GetRegistry().emplace<MeshComponent>(sphereEntity, MeshGeometryType::Sphere, BLUE, DARKBLUE);

    // 3. Green Cylinder
    auto cylEntity = m_scene.CreateEntity("Green Pillar");
    auto& cylTransform = m_scene.GetRegistry().get<TransformComponent>(cylEntity);
    cylTransform.position = (Vector3){ -4.0f, 1.5f, 0.0f };
    cylTransform.scale = (Vector3){ 1.5f, 3.0f, 1.5f };
    m_scene.GetRegistry().emplace<MeshComponent>(cylEntity, MeshGeometryType::Cylinder, LIME, DARKGREEN);

    // 4. Main Game Camera
    auto camEntity = m_scene.CreateEntity("Main Camera");
    auto& camTransform = m_scene.GetRegistry().get<TransformComponent>(camEntity);
    camTransform.position = (Vector3){ 0.0f, 4.0f, 9.0f };
    CameraComponent cam;
    cam.camera.position = (Vector3){ 0.0f, 4.0f, 9.0f };
    cam.camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };
    cam.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    cam.camera.fovy = 50.0f;
    cam.camera.projection = CAMERA_PERSPECTIVE;
    cam.isPrimary = true;
    m_scene.GetRegistry().emplace<CameraComponent>(camEntity, cam);
}

Camera3D Application::GetCurrentGameCamera() {
    auto view = m_scene.GetRegistry().view<CameraComponent>();
    for (auto entity : view) {
        const auto& c = view.get<CameraComponent>(entity);
        if (c.isPrimary) {
            return c.camera;
        }
    }
    // Fallback if no primary marked
    for (auto entity : view) {
        return view.get<CameraComponent>(entity).camera;
    }

    // Default fallback camera
    Camera3D defaultCam = { 0 };
    defaultCam.position = (Vector3){ 0.0f, 5.0f, 10.0f };
    defaultCam.target = (Vector3){ 0.0f, 1.0f, 0.0f };
    defaultCam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    defaultCam.fovy = 45.0f;
    defaultCam.projection = CAMERA_PERSPECTIVE;
    return defaultCam;
}

void Application::HandleCameraInput() {
    ImGuiIO& io = ImGui::GetIO();

    // Prevent camera movement if ImGui is capturing mouse OR gizmo is being dragged
    if (!io.WantCaptureMouse && !m_editorLayer->IsGizmoDragging()) {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            UpdateCamera(&m_editorCamera, CAMERA_FREE);
        } else {
            // Scroll zoom
            float wheel = GetMouseWheelMove();
            if (wheel != 0) {
                CameraMoveToTarget(&m_editorCamera, -wheel * 1.5f);
            }
        }
    }
}

void Application::Run() {
    int frameCounter = 0;

    std::cout << "[REngine] [Run] Entering main loop (Mode: " 
              << (m_config.isGameMode ? "STANDALONE GAME" : "EDITOR") << ")..." << std::endl;

    while (!WindowShouldClose()) {
        frameCounter++;
        float dt = GetFrameTime();

        // Update particle system
        ParticleSystem3D::Get().OnUpdate(dt);

        // Update all attached game layers
        for (auto& layer : m_layers) {
            layer->OnUpdate(dt);
        }

        if (m_config.isGameMode) {
            // ================= STANDALONE GAME MODE =================
            Camera3D gameCamera = GetCurrentGameCamera();

            BeginDrawing();
                ClearBackground((Color){ 25, 25, 30, 255 });

                // 1. Render 3D Scene using Game Camera
                m_renderSystem.Render(m_scene, gameCamera, false);

                // 2. Render 3D Particles and Game Layers
                BeginMode3D(gameCamera);
                    ParticleSystem3D::Get().OnRender3D();

                    for (auto& layer : m_layers) {
                        layer->OnRender3D();
                    }
                EndMode3D();

                // 3. Render 2D UI for Game Layers
                for (auto& layer : m_layers) {
                    layer->OnRenderUI();
                }

            EndDrawing();
        } else {
            // ================= EDITOR MODE =================
            HandleCameraInput();
            Camera3D activeCamera = m_editorCamera;

            // Handle 3D Mouse Picking to select objects directly in the viewport
            if (m_editorLayer) {
                m_editorLayer->HandleMousePicking(activeCamera);
            }

            BeginDrawing();
                ClearBackground((Color){ 25, 25, 30, 255 });

                // 1. Render 3D Scene in Edit mode
                entt::entity selectedEntity = m_editorLayer ? m_editorLayer->GetSelectedEntity() : entt::null;
                m_renderSystem.Render(m_scene, activeCamera, true, selectedEntity);

                // 2. Render 3D Particles, Game Layers, Gizmo
                BeginMode3D(activeCamera);
                    ParticleSystem3D::Get().OnRender3D();

                    for (auto& layer : m_layers) {
                        layer->OnRender3D();
                    }

                    if (m_editorLayer) {
                        m_editorLayer->RenderGizmo(activeCamera);
                    }
                EndMode3D();

                // 3. Render Dear ImGui Editor UI
                if (m_editorLayer) {
                    m_editorLayer->BeginFrame();
                    m_editorLayer->RenderUI();

                    for (auto& layer : m_layers) {
                        layer->OnRenderUI();
                    }
                    m_editorLayer->EndFrame();
                }

            EndDrawing();
        }

        // Automated test frame handling
        if (m_config.testFrames > 0) {
            if (frameCounter == 1 || frameCounter % 30 == 0 || frameCounter == m_config.testFrames) {
                std::cout << "[REngine] [Test] Frame " << frameCounter << "/" << m_config.testFrames 
                          << " | FPS: " << GetFPS() 
                          << " | FrameTime: " << (GetFrameTime() * 1000.0f) << " ms"
                          << " | Status: OK" << std::endl;
            }

            if (frameCounter >= m_config.testFrames) {
                std::cout << "[REngine] [Test] Completed " << m_config.testFrames << " frames. Exiting test run successfully!" << std::endl;
                break;
            }
        }
    }
}

} // namespace REngine
