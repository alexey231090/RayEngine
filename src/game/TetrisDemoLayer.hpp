#pragma once

#include "core/Layer.hpp"
#include "audio/SoundFX.hpp"
#include "renderer/ParticleSystem3D.hpp"
#include "raylib.h"
#include "imgui.h"

namespace REngine {

class TetrisDemoLayer : public Layer {
public:
    TetrisDemoLayer() : Layer("TetrisDemoLayer") {}

    void OnAttach() override {
        // Ready for game logic
    }

    void OnUpdate(float dt) override {
        // Space key triggers particle explosion and sound
        if (IsKeyPressed(KEY_SPACE)) {
            Vector3 burstPos = { 0.0f, 2.0f, 0.0f };
            ParticleSystem3D::Get().EmitBurst(burstPos, GOLD, 50);
            SoundFX::PlayLineClear();
            m_score += 100;
        }

        // Click sound on Left Mouse Click in game view
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ImGui::GetIO().WantCaptureMouse) {
            Ray ray = GetMouseRay(GetMousePosition(), GetCamera());
            Vector3 hitPos = { ray.position.x + ray.direction.x * 5.0f, ray.position.y + ray.direction.y * 5.0f, ray.position.z + ray.direction.z * 5.0f };
            ParticleSystem3D::Get().EmitBurst(hitPos, SKYBLUE, 25);
            SoundFX::PlayClick();
        }
    }

    void OnRender3D() override {
        // Draw demo game board boundary grid
        DrawCubeWires((Vector3){ 0.0f, 5.0f, 0.0f }, 10.0f, 10.0f, 2.0f, (Color){ 100, 100, 120, 100 });
    }

    void OnRenderUI() override {
        ImGui::SetNextWindowPos(ImVec2(10, 220), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(240, 120), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Game Layer: 3D Tetris FX")) {
            ImGui::Text("Score: %d", m_score);
            ImGui::Separator();
            if (ImGui::Button("Spawn FX Explosion")) {
                ParticleSystem3D::Get().EmitBurst((Vector3){ 0.0f, 2.0f, 0.0f }, ORANGE, 60);
                SoundFX::PlayExplosion();
                m_score += 50;
            }
            ImGui::TextDisabled("Press SPACE to clear line");
        }
        ImGui::End();
    }

private:
    Camera3D GetCamera() {
        Camera3D cam = { 0 };
        cam.position = (Vector3){ 0.0f, 8.0f, 14.0f };
        cam.target = (Vector3){ 0.0f, 1.0f, 0.0f };
        cam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        cam.fovy = 45.0f;
        cam.projection = CAMERA_PERSPECTIVE;
        return cam;
    }

private:
    int m_score = 0;
};

} // namespace REngine
