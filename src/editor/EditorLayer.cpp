#include "EditorLayer.hpp"
#include "imgui.h"
#include "rlImGui.h"
#include "scene/SceneSerializer.hpp"
#include "scene/Components.hpp"
#include <iostream>

namespace REngine {

EditorLayer::EditorLayer(Scene& scene) : m_scene(scene) {}

EditorLayer::~EditorLayer() {}

void EditorLayer::Init() {
    rlImGuiSetup(true);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}

void EditorLayer::Shutdown() {
    rlImGuiShutdown();
}

void EditorLayer::BeginFrame() {
    rlImGuiBegin();
}

void EditorLayer::EndFrame() {
    rlImGuiEnd();
}

void EditorLayer::RenderGizmo(const Camera3D& camera) {
    if (m_mode != EngineMode::Edit) return;

    auto& registry = m_scene.GetRegistry();
    if (m_selectedEntity != entt::null && registry.valid(m_selectedEntity)) {
        if (registry.all_of<TransformComponent>(m_selectedEntity)) {
            auto& transform = registry.get<TransformComponent>(m_selectedEntity);
            bool allowInteraction = !ImGui::GetIO().WantCaptureMouse;
            m_gizmo.UpdateAndRender(camera, transform.position, allowInteraction);
        }
    }
}

void EditorLayer::RenderUI() {
    DrawToolbarPanel();
    DrawHierarchyPanel();
    DrawInspectorPanel();
    DrawStatsPanel();
}

void EditorLayer::DrawToolbarPanel() {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(520, 100), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Engine Controls & Simulation")) {
        // Play / Stop buttons
        if (m_mode == EngineMode::Edit) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.65f, 0.25f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.80f, 0.35f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.10f, 0.50f, 0.20f, 1.0f));

            if (ImGui::Button("  ▶ Play  ", ImVec2(100, 32))) {
                m_mode = EngineMode::Play;
                m_modeChanged = true;
                m_statusMessage = "Game Simulation Started";
            }
            ImGui::PopStyleColor(3);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.80f, 0.20f, 0.20f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.30f, 0.30f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.60f, 0.15f, 0.15f, 1.0f));

            if (ImGui::Button("  ⏹ Stop  ", ImVec2(100, 32))) {
                m_mode = EngineMode::Edit;
                m_modeChanged = true;
                m_statusMessage = "Game Stopped - State Restored";
            }
            ImGui::PopStyleColor(3);
        }

        ImGui::SameLine();
        ImGui::TextUnformatted("|");
        ImGui::SameLine();

        // Save and Load Scene
        if (ImGui::Button("Save JSON")) {
            SceneSerializer serializer(m_scene);
            if (serializer.Serialize(m_sceneFilePath)) {
                m_statusMessage = "Scene saved to " + m_sceneFilePath;
            } else {
                m_statusMessage = "Failed to save scene!";
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Load JSON")) {
            SceneSerializer serializer(m_scene);
            if (serializer.Deserialize(m_sceneFilePath)) {
                m_selectedEntity = entt::null;
                m_statusMessage = "Scene loaded from " + m_sceneFilePath;
            } else {
                m_statusMessage = "Failed to load scene!";
            }
        }

        // Status text and current mode
        ImGui::SameLine();
        if (m_mode == EngineMode::Play) {
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.4f, 1.0f), "[PLAYING]");
        } else {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "[EDIT MODE]");
        }

        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Status: %s", m_statusMessage.c_str());
    }
    ImGui::End();
}

void EditorLayer::DrawHierarchyPanel() {
    ImGui::SetNextWindowPos(ImVec2(10, 120), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(280, 420), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Scene Hierarchy")) {
        if (ImGui::Button("+ Cube")) {
            auto entity = m_scene.CreateEntity("Cube");
            m_scene.GetRegistry().emplace<MeshComponent>(entity, MeshGeometryType::Cube, RED, MAROON);
            m_selectedEntity = entity;
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Sphere")) {
            auto entity = m_scene.CreateEntity("Sphere");
            m_scene.GetRegistry().emplace<MeshComponent>(entity, MeshGeometryType::Sphere, BLUE, DARKBLUE);
            m_selectedEntity = entity;
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Cylinder")) {
            auto entity = m_scene.CreateEntity("Cylinder");
            m_scene.GetRegistry().emplace<MeshComponent>(entity, MeshGeometryType::Cylinder, GREEN, DARKGREEN);
            m_selectedEntity = entity;
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Camera")) {
            auto entity = m_scene.CreateEntity("Game Camera");
            CameraComponent cam;
            cam.camera.position = (Vector3){ 0.0f, 5.0f, 10.0f };
            cam.camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };
            m_scene.GetRegistry().emplace<CameraComponent>(entity, cam);
            m_selectedEntity = entity;
        }

        ImGui::Separator();

        auto& registry = m_scene.GetRegistry();
        auto view = registry.view<TagComponent>();

        for (auto entity : view) {
            const auto& tag = view.get<TagComponent>(entity);
            bool isSelected = (m_selectedEntity == entity);

            std::string icon = "  ";
            if (registry.all_of<CameraComponent>(entity)) icon = "[Cam] ";
            else if (registry.all_of<MeshComponent>(entity)) icon = "[Obj] ";

            std::string label = icon + tag.tag + " ##" + std::to_string((uint32_t)entity);
            if (ImGui::Selectable(label.c_str(), isSelected)) {
                m_selectedEntity = entity;
            }
        }

        ImGui::Separator();

        if (m_selectedEntity != entt::null && registry.valid(m_selectedEntity)) {
            if (ImGui::Button("Delete Selected Entity")) {
                m_scene.DestroyEntity(m_selectedEntity);
                m_selectedEntity = entt::null;
            }
        }
    }
    ImGui::End();
}

void EditorLayer::DrawInspectorPanel() {
    ImGui::SetNextWindowPos(ImVec2(980, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(290, 530), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Entity Inspector")) {
        auto& registry = m_scene.GetRegistry();

        if (m_selectedEntity == entt::null || !registry.valid(m_selectedEntity)) {
            ImGui::TextDisabled("No entity selected");
            ImGui::End();
            return;
        }

        // Tag / Name
        if (registry.all_of<TagComponent>(m_selectedEntity)) {
            auto& tag = registry.get<TagComponent>(m_selectedEntity);
            char buffer[256];
            strncpy(buffer, tag.tag.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';
            if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
                tag.tag = buffer;
            }
        }

        ImGui::Separator();

        // Transform Component
        if (registry.all_of<TransformComponent>(m_selectedEntity)) {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& transform = registry.get<TransformComponent>(m_selectedEntity);

                float pos[3] = { transform.position.x, transform.position.y, transform.position.z };
                if (ImGui::DragFloat3("Position", pos, 0.05f)) {
                    transform.position = { pos[0], pos[1], pos[2] };
                }

                float rot[3] = { transform.rotation.x, transform.rotation.y, transform.rotation.z };
                if (ImGui::DragFloat3("Rotation", rot, 1.0f)) {
                    transform.rotation = { rot[0], rot[1], rot[2] };
                }

                float scale[3] = { transform.scale.x, transform.scale.y, transform.scale.z };
                if (ImGui::DragFloat3("Scale", scale, 0.05f, 0.01f, 100.0f)) {
                    transform.scale = { scale[0], scale[1], scale[2] };
                }
            }
        }

        // Mesh Component
        if (registry.all_of<MeshComponent>(m_selectedEntity)) {
            ImGui::Separator();
            if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& mesh = registry.get<MeshComponent>(m_selectedEntity);

                const char* geometries[] = { "Cube", "Sphere", "Cylinder", "Plane" };
                int currentType = (int)mesh.geometryType;
                if (ImGui::Combo("Geometry", &currentType, geometries, IM_ARRAYSIZE(geometries))) {
                    mesh.geometryType = (MeshGeometryType)currentType;
                }

                float col[4] = {
                    mesh.color.r / 255.0f,
                    mesh.color.g / 255.0f,
                    mesh.color.b / 255.0f,
                    mesh.color.a / 255.0f
                };
                if (ImGui::ColorEdit4("Color", col)) {
                    mesh.color = {
                        (unsigned char)(col[0] * 255.0f),
                        (unsigned char)(col[1] * 255.0f),
                        (unsigned char)(col[2] * 255.0f),
                        (unsigned char)(col[3] * 255.0f)
                    };
                }

                ImGui::Checkbox("Wireframe", &mesh.drawWires);

                if (mesh.drawWires) {
                    float wireCol[4] = {
                        mesh.wireColor.r / 255.0f,
                        mesh.wireColor.g / 255.0f,
                        mesh.wireColor.b / 255.0f,
                        mesh.wireColor.a / 255.0f
                    };
                    if (ImGui::ColorEdit4("Wire Color", wireCol)) {
                        mesh.wireColor = {
                            (unsigned char)(wireCol[0] * 255.0f),
                            (unsigned char)(wireCol[1] * 255.0f),
                            (unsigned char)(wireCol[2] * 255.0f),
                            (unsigned char)(wireCol[3] * 255.0f)
                        };
                    }
                }

                if (ImGui::Button("Remove Mesh Component")) {
                    registry.remove<MeshComponent>(m_selectedEntity);
                }
            }
        }

        // Camera Component
        if (registry.all_of<CameraComponent>(m_selectedEntity)) {
            ImGui::Separator();
            if (ImGui::CollapsingHeader("Camera Component", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& cam = registry.get<CameraComponent>(m_selectedEntity);

                float cpos[3] = { cam.camera.position.x, cam.camera.position.y, cam.camera.position.z };
                if (ImGui::DragFloat3("Cam Position", cpos, 0.1f)) {
                    cam.camera.position = { cpos[0], cpos[1], cpos[2] };
                }

                float ctarget[3] = { cam.camera.target.x, cam.camera.target.y, cam.camera.target.z };
                if (ImGui::DragFloat3("Cam Target", ctarget, 0.1f)) {
                    cam.camera.target = { ctarget[0], ctarget[1], ctarget[2] };
                }

                ImGui::SliderFloat("FOV", &cam.camera.fovy, 20.0f, 120.0f);
                ImGui::Checkbox("Primary Camera", &cam.isPrimary);

                if (ImGui::Button("Remove Camera Component")) {
                    registry.remove<CameraComponent>(m_selectedEntity);
                }
            }
        }
    }
    ImGui::End();
}

void EditorLayer::DrawStatsPanel() {
    ImGui::SetNextWindowPos(ImVec2(10, 550), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(280, 150), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Performance & Stats")) {
        ImGui::Text("FPS: %d", GetFPS());
        ImGui::Text("Frame Time: %.2f ms", GetFrameTime() * 1000.0f);
        ImGui::Separator();
        auto& registry = m_scene.GetRegistry();
        size_t entityCount = registry.storage<entt::entity>().size();
        ImGui::Text("Active Entities: %zu", entityCount);
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.9f, 0.8f, 0.3f, 1.0f), "Controls:");
        ImGui::Text("Click arrows to drag object");
        ImGui::Text("Hold RMB to orbit camera");
    }
    ImGui::End();
}

} // namespace REngine
