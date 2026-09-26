# AGENTS.md — AI-Friendly C++ Game Engine Guidelines

## Project Mission
This project is an **AI-First lightweight 3D Game Engine & Editor** built with **C++17**, **Raylib 5.0**, **EnTT (ECS)**, and **Dear ImGui**.
It is specifically optimized for autonomous AI coding agents (Antigravity, Claude Code, Cursor, Codex) to build, test, and extend games without getting stuck or hanging.

---

## Technical Stack & Dependencies
- **Language**: Modern C++17 (RAII, STL, zero raw memory ownership).
- **Core Graphics/Audio/Windowing**: [Raylib 5.0](https://www.raylib.com/) (pinned release ZIP, 100% Git-independent).
- **ECS (Entity Component System)**: [EnTT](https://github.com/skypjack/entt) v3.13.2 (pure POD components).
- **Scene Serialization**: [nlohmann/json](https://github.com/nlohmann/json) v3.11.3 (declarative `scene.json`).
- **GUI & Editor**: [Dear ImGui](https://github.com/ocornut/imgui) (docking branch) + [rlImGui](src/rlImGui) (embedded).
- **Audio System**: Procedural audio synthesizer `SoundFX` (zero audio assets on disk needed).
- **VFX System**: `ParticleSystem3D` (real-time 3D particle bursts and debris).
- **Build System**: CMake (>= 3.16) with Ninja and `build.bat` (with auto-cache clearing and Windows Defender retry logic).

---

## AI Agent Skill (Superpower for LLMs)
For instant game creation workflows, consult the specialized AI Skill file:
`[rengine-game-dev](file:///.agents/skills/rengine-game-dev/SKILL.md)`
It contains complete copy-paste templates, API cheatsheets, and recipes for building games in 1 prompt.

---

## Build Commands for AI Agents

### Recommended: Self-Contained Batch Build (Windows)
```cmd
.\build.bat
```
- **Self-healing cache**: If the repository folder was copied or renamed, `build.bat` automatically purges stale `CMakeCache.txt` paths.
- **Windows Defender resilient**: Includes automatic retry handling with exponential backoff if antivirus scans briefly lock `gcc` or `as.exe`.

### Alternative: Standard CMake Preset
```bash
cmake --preset default
cmake --build --preset default
```

---

## Automated Verification & Testing (CRITICAL FOR AI)

AI agents do NOT have human hands to close interactive game windows. Running `RaylibEngineApp.exe` without test flags will freeze your terminal session and lock the `.exe` file.

### Safe Autonomous Test Run
Always verify your code changes by running the engine with `--test-frames`:
```bash
.\build\RaylibEngineApp.exe --test-frames 60
```
- **What it does**: Initializes the graphics context, loads the scene and game layers, runs the main loop for exactly 60 frames, prints structured telemetry to stdout, and exits with code `0`.
- **Headless mode** (hidden window for background runners / CI):
```bash
.\build\RaylibEngineApp.exe --test-frames 60 --headless
```

### Console Telemetry Format
The engine prints high-level milestones to stdout:
```text
[REngine] [Init] Window initialized: 1280x720 @ 60 FPS target
[REngine] [Audio] SoundFX procedural audio initialized successfully.
[REngine] [Scene] Successfully loaded 'scene.json' with 3 active entities
[REngine] [Layer] Attached layer: 'TetrisDemoLayer'
[REngine] [Test] Frame 60/60 | FPS: 60 | FrameTime: 16.6 ms | Status: OK
[REngine] [Test] Completed 60 frames. Exiting test run successfully!
[REngine] [Shutdown] Engine shut down cleanly with exit code 0
```

---

## How AI Agents Build Games in 1 Minute

### 1. Game Layer Architecture (`src/core/Layer.hpp`)
**Never edit engine core files (`Application.cpp`)!**
Instead, create a self-contained game layer in `src/game/YourGameLayer.hpp`:
```cpp
#pragma once
#include "core/Layer.hpp"
#include "audio/SoundFX.hpp"
#include "renderer/ParticleSystem3D.hpp"
#include "raylib.h"

class MyGameLayer : public Layer {
public:
    void OnAttach() override {}
    void OnUpdate(float dt) override {
        if (IsKeyPressed(KEY_SPACE)) {
            SoundFX::PlayCoin();
            ParticleSystem3D::Instance().EmitBurst({0, 2, 0}, 20, GOLD);
        }
    }
    void OnRender3D() override {
        DrawCube({0, 0, 0}, 2, 2, 2, RED);
    }
    void OnRenderUI() override {
        ImGui::Begin("HUD");
        ImGui::Text("Press SPACE for FX!");
        ImGui::End();
    }
};
```
Then plug it into `src/main.cpp`:
```cpp
app.PushLayer<MyGameLayer>();
```

### 2. Procedural Audio (`SoundFX`)
Play sounds directly from C++ without generating `.wav` files:
- `SoundFX::PlayClick()` (UI / Move)
- `SoundFX::PlayCoin()` (Score / Pickup)
- `SoundFX::PlayFall()` (Hard drop / Landing)
- `SoundFX::PlayExplosion()` (Damage / Line Burn)
- `SoundFX::PlayLineClear()` (Level up / Combo)
- `SoundFX::PlayTone(freq, duration)` (Procedural tone synth)

### 3. 3D Particles (`ParticleSystem3D`)
Add immediate juice and explosions:
```cpp
ParticleSystem3D::Instance().EmitBurst({x, y, z}, 25, ORANGE, 5.0f, 0.3f);
```

### 4. Declarative World Editing (`scene.json`)
To add static meshes or level geometry, edit `scene.json` directly. The engine serializes and deserializes entities, transforms, geometry, colors, and camera settings automatically.

---

## Critical Rules for AI Agents

1. **Always verify with `--test-frames 60`**:
   Before claiming a task is complete, run `build.bat` followed by `.\build\RaylibEngineApp.exe --test-frames 60` to guarantee 0 compiler errors and 0 runtime crashes.
2. **Never leave orphan processes running**:
   Always use `--test-frames` to avoid `CreateProcess: Access is denied` file lock errors on subsequent builds.
3. **No hardcoded machine-specific absolute paths**:
   All paths in CMake and C++ must remain relative to project root.
