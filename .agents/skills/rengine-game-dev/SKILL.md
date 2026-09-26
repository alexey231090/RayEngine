---
name: rengine-game-dev
description: Expert AI workflow for building 3D games, mechanics, audio, and visual effects in REngine (Raylib 5.0 + EnTT ECS + ImGui) without modifying engine core.
---

# REngine Game Development Skill for AI Agents

This skill enables AI coding agents (Antigravity, Claude Code, Cursor, Codex) to build complete, polished 3D games (like 3D Tetris, Snake, Space Invaders, Breakout) in REngine within **1–2 prompts**, without breaking or touching engine core code.

---

## 1. Golden Rules for AI Game Developers

1. **NEVER modify engine core files**:
   - Do NOT edit `src/core/Application.cpp` or `src/core/Application.hpp`.
   - Do NOT edit `src/editor/*` or `src/scene/*`.
2. **Build your game in a dedicated Layer**:
   - Create a single self-contained header/class in `src/game/YourGameLayer.hpp`.
   - Clear the demo entities and register your layer in `src/main.cpp`:
     ```cpp
     app.ClearScene(); // Clears demo cube/sphere/pillar, preserves camera
     app.PushLayer<YourGameLayer>();
     ```
3. **Always verify with `--test-frames 60`**:
   - Build: `.\build.bat`
   - Test: `.\build\RaylibEngineApp.exe --game --test-frames 60 --headless`
   - Never run `RaylibEngineApp.exe` interactively without test flags — it will freeze your terminal.

---

## 2. Standard GameLayer Template

Every game in REngine is a subclass of [Layer](src/core/Layer.hpp):

```cpp
#pragma once
#include "core/Layer.hpp"
#include "audio/SoundFX.hpp"
#include "renderer/ParticleSystem3D.hpp"
#include "ui/EngineFont.hpp"
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"

class MyGameLayer : public Layer {
public:
    void OnAttach() override {
        // Direct camera & scene setup from inside Layer:
        Camera3D cam = GetPrimaryCamera();
        cam.position = (Vector3){ 0.0f, 10.0f, 12.0f };
        cam.target = (Vector3){ 0.0f, 0.0f, 0.0f };
        SetPrimaryCamera(cam);

        ResetGame();
    }

    void OnUpdate(float dt) override {
        // 1. Handle Input
        if (IsKeyPressed(KEY_SPACE)) {
            // Action
            SoundFX::PlayClick();
            ParticleSystem3D::Instance().EmitBurst({0.0f, 1.0f, 0.0f}, 15, GOLD);
        }

        // 2. Update logic, timers, physics
    }

    void OnRender3D() override {
        // Render 3D geometry (automatically lit with 3D directional sunlight & ambient shading)
        DrawGrid(10, 1.0f);
        DrawCube({0.0f, 0.5f, 0.0f}, 1.0f, 1.0f, 1.0f, RED);
        DrawCubeWires({0.0f, 0.5f, 0.0f}, 1.0f, 1.0f, 1.0f, MAROON);
    }

    void OnRenderUI() override {
        // 1. Raylib 2D text with native Cyrillic (UTF-8) support:
        DrawTextUTF8("Счет: 100", 20, 20, 24, WHITE);

        // 2. Dear ImGui HUD (safely works in BOTH Editor and Standalone --game modes!):
        ImGui::Begin("Game HUD", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Счет: %d", m_Score);
        if (ImGui::Button("Перезапуск")) {
            ResetGame();
        }
        ImGui::End();
    }

    void OnDetach() override {
        // Cleanup if needed
    }

private:
    void ResetGame() {
        m_Score = 0;
    }

    int m_Score = 0;
};
```

---

## 3. Direct Engine Context Access in Layer

Every `Layer` has direct access methods without needing to pass pointers around:
- `GetScene()`: Access to the ECS `Scene&` to spawn or destroy entities.
- `GetPrimaryCamera()`: Get current `Camera3D`.
- `SetPrimaryCamera(cam)`: Set game camera position and target.
- `GetApp()`: Access to the main `Application&`.

---

## 4. Built-in Procedural Audio (`SoundFX`)

No `.wav` or audio files on disk required! REngine provides an instant procedural sound generator:

```cpp
#include "audio/SoundFX.hpp"

// Instant preset sound effects:
SoundFX::PlayClick();       // Menu clicks, piece movement, tap
SoundFX::PlayCoin();        // Point scored, bonus collected, pickup
SoundFX::PlayFall();        // Tetromino hard drop, object landing
SoundFX::PlayExplosion();   // Bomb blast, game over, line burn
SoundFX::PlayLineClear();   // Combo, line cleared, level completed

// Custom procedural frequency synth:
SoundFX::PlayTone(440.0f, 0.15f); // 440 Hz tone for 0.15 seconds
```

---

## 5. 3D Particle System (`ParticleSystem3D`)

For instant visual feedback (explosions, block clears, sparks):

```cpp
#include "renderer/ParticleSystem3D.hpp"

// Instant burst of cubes/sparks:
Vector3 pos = {0.0f, 5.0f, 0.0f};
ParticleSystem3D::Instance().EmitBurst(
    pos,          // Center position (Vector3)
    25,           // Particle count
    ORANGE,       // Color (Raylib Color: RED, GOLD, SKYBLUE, VIOLET, etc.)
    5.0f,         // Explosion velocity speed
    0.3f          // Cube particle size
);
```

---

## 5. 3D Math & Raylib Utilities Cheatsheet

- **Vector Math**:
  - `Vector3Add(a, b)`
  - `Vector3Subtract(a, b)`
  - `Vector3Scale(v, scale)`
  - `Vector3Distance(a, b)`
- **Raylib Input**:
  - `IsKeyPressed(KEY_UP)`, `IsKeyDown(KEY_W)`
  - `IsMouseButtonPressed(MOUSE_BUTTON_LEFT)`
  - `GetMousePosition()`, `GetMouseRay(GetMousePosition(), camera)`
- **3D Primitives**:
  - `DrawCube(pos, width, height, length, color)`
  - `DrawCubeWires(pos, width, height, length, wireColor)`
  - `DrawSphere(pos, radius, color)`
  - `DrawCylinder(pos, radiusTop, radiusBottom, height, slices, color)`
  - `DrawGrid(slices, spacing)`

---

## 6. How to Plug In Your Game

In `src/main.cpp`:

```cpp
#include "core/Application.hpp"
#include "game/YourGameLayer.hpp" // 1. Include your layer

int main(int argc, char* argv[]) {
    REngine::AppConfig config;
    REngine::Application app(config);
    
    // 2. Clear demo scene entities (cube, sphere, cylinder)
    app.ClearScene();
    
    // 3. Push your game layer onto the stack
    app.PushLayer<YourGameLayer>();
    
    app.Run();
    return 0;
}
```

---

## 7. Verification Routine

Always execute:
```bash
.\build.bat
.\build\RaylibEngineApp.exe --test-frames 60
```
Check that the console prints:
```text
[REngine] [Test] Completed 60 frames. Exiting test run successfully!
[REngine] [Shutdown] Engine shut down cleanly with exit code 0
```
