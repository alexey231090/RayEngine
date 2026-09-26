# REngine — AI-First 3D Engine & ImGui Editor

A lightweight, modern 3D Game Engine built on top of **C++17**, **[Raylib 5.0](https://www.raylib.com/)**, **EnTT (ECS)**, and **Dear ImGui**, designed for lightning-fast game prototyping by both human developers and autonomous AI coding agents.

---

## Key Features

- **Game Layer Architecture (`Layer`)**: Decouple game logic completely from the engine core! Build complete games like 3D Tetris, Snake, or Space Invaders in a single `Layer` class (`PushLayer<YourGame>()`) without touching `Application.cpp`.
- **Procedural Audio Synthesizer (`SoundFX`)**: Zero audio assets on disk needed. Play procedural sound effects (`PlayClick`, `PlayCoin`, `PlayFall`, `PlayExplosion`, `PlayLineClear`, `PlayTone`) instantly from code.
- **3D Particle System (`ParticleSystem3D`)**: Spawn real-time 3D particle bursts, explosions, and debris with a single line of code (`ParticleSystem3D::Instance().EmitBurst(...)`).
- **AI Agent Skill (`.agents/skills/rengine-game-dev/SKILL.md`)**: Built-in instructions, code templates, and API cheatsheets for Antigravity, Claude Code, Cursor, and Codex to build games in 1 prompt.
- **Entity Component System (ECS)**: Powered by `EnTT`, keeping data decoupled into plain structures (`TagComponent`, `TransformComponent`, `MeshComponent`, `CameraComponent`).
- **AI-First JSON Scene Format**: Scenes are fully serialized to and loaded from `scene.json` using `nlohmann/json`. AI agents can read, modify, or generate complete scenes without recompiling C++.
- **Automated Testing for AI Agents**:
  - `--test-frames <N>`: Runs the game for $N$ frames, prints structured telemetry to stdout, and exits with code `0`. Prevents process hanging and file lock issues (`Access is denied`)!
  - `--headless`: Runs with a hidden window, ideal for CI pipelines and headless AI background verification.
  - `--scene <path>`: Loads a specific scene file.
- **Visual Editor for Humans**: Embedded **Dear ImGui**:
  - **3D Gizmo Translators**: Interactive X, Y, Z coordinate arrows to drag objects with mouse in 3D viewport (just like in Unity).
  - **Simulation Modes (Play / Stop)**: Toggle between `Edit Mode` (free camera & gizmos) and `Play Mode` (game camera view with automatic scene state restoration).
  - **Scene Hierarchy & Inspector**: Browse, spawn, delete entities, and live-edit transforms, colors, and camera FOV.
  - **Hot Reload**: One-click `Save JSON` and `Load JSON`.
- **Zero Manual Pre-installation & Git Independence**: Raylib 5.0, EnTT, nlohmann_json, and Dear ImGui are automatically resolved as release ZIP archives via CMake `FetchContent`.
- **Self-Healing Build System**: `build.bat` automatically invalidates stale CMake paths if the project directory is moved or copied, and handles Windows Defender lock recovery.

---

## Build & Run

### Quick Build (Windows)
```cmd
.\build.bat
.\run.bat
```

### Standard CMake Presets (Cross-Platform)
```bash
cmake --preset default
cmake --build --preset default
```

### AI Agent Automated Verification Run
```bash
.\build\RaylibEngineApp.exe --test-frames 60
```
or in headless mode:
```bash
.\build\RaylibEngineApp.exe --test-frames 60 --headless
```

---

## Controls

- **Left Mouse Button (LKM)**: Drag Gizmo arrows to move objects; interact with editor UI.
- **Right Mouse Button (RMB) + Drag**: Orbit / Free-look 3D Camera in the viewport.
- **Mouse Wheel**: Zoom camera towards target.
- **Close Window / ESC**: Exit application.

---

## Project Structure

```
REngine/
├── .agents/skills/
│   └── rengine-game-dev/
│       └── SKILL.md              # AI Skill cheatsheet for instant game creation
├── src/
│   ├── main.cpp                  # Application entry point & layer registration
│   ├── core/
│   │   ├── Application.hpp       # Main engine loop and Raylib window lifecycle
│   │   ├── Application.cpp
│   │   └── Layer.hpp             # Base Layer interface (OnAttach, OnUpdate, OnRender3D, OnRenderUI)
│   ├── audio/
│   │   ├── SoundFX.hpp           # Procedural sound effect synthesizer
│   │   └── SoundFX.cpp
│   ├── renderer/
│   │   ├── RenderSystem.hpp      # 3D primitives and camera frustum rendering
│   │   ├── RenderSystem.cpp
│   │   ├── ParticleSystem3D.hpp  # 3D particle emitter and explosion system
│   │   └── ParticleSystem3D.cpp
│   ├── game/
│   │   └── TetrisDemoLayer.hpp   # 3D Tetris demo layer showcasing audio, particles & HUD
│   ├── scene/
│   │   ├── Components.hpp        # ECS POD components (Transform, Mesh, Tag, Camera)
│   │   ├── Scene.hpp             # EnTT registry wrapper and entity management
│   │   ├── Scene.cpp
│   │   ├── SceneSerializer.hpp   # JSON scene serializer/deserializer for AI agents
│   │   └── SceneSerializer.cpp
│   ├── editor/
│   │   ├── Gizmo.hpp             # 3D translation Gizmo arrows
│   │   ├── Gizmo.cpp
│   │   ├── EditorLayer.hpp       # Dear ImGui editor panels & Play/Stop simulation
│   │   └── EditorLayer.cpp
│   └── rlImGui/                  # Embedded Raylib backend for Dear ImGui
├── scene.json                    # Declarative world state readable and editable by AI
├── CMakeLists.txt                # CMake project build definition (ZIP FetchContent)
├── CMakePresets.json             # Standard CMake presets configuration
├── AGENTS.md                     # AI agent instructions and conventions
├── README.md                     # Documentation
├── build.bat                     # Windows build script with self-healing cache
└── run.bat                       # Windows run script
```
