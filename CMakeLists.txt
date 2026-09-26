cmake_minimum_required(VERSION 3.16)

# Set minimum policy version for third-party libraries (like Raylib) under CMake 4.x
set(CMAKE_POLICY_VERSION_MINIMUM "3.5" CACHE STRING "" FORCE)

# Enable direct download mode for FetchContent (CMake >= 3.30) and modern timestamps
if(POLICY CMP0168)
    cmake_policy(SET CMP0168 NEW)
endif()
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# Prevent Defender file locks on try_compile
set(CMAKE_C_COMPILER_WORKS TRUE CACHE BOOL "" FORCE)
set(CMAKE_CXX_COMPILER_WORKS TRUE CACHE BOOL "" FORCE)

project(AI_RaylibEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure build options before FetchContent
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(ENTT_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)

include(FetchContent)

# 1. Raylib (Fixed Stable 5.0 Release ZIP Archive - 100% Git-independent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.zip
)

# 2. EnTT (Fixed Stable v3.13.2 Release ZIP Archive)
FetchContent_Declare(
    entt
    URL https://github.com/skypjack/entt/archive/refs/tags/v3.13.2.zip
)

# 3. nlohmann_json (Fixed Stable v3.11.3 Release ZIP Archive)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip
)

# 4. Dear ImGui (Fixed Stable Docking Branch ZIP Archive)
FetchContent_Declare(
    imgui
    URL https://github.com/ocornut/imgui/archive/refs/heads/docking.zip
)

FetchContent_MakeAvailable(raylib entt nlohmann_json imgui)

# ImGui sources
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)

# Engine sources (includes src/rlImGui/rlImGui.cpp, core, scene, editor, renderer, audio)
file(GLOB_RECURSE ENGINE_SOURCES
    src/*.cpp
    src/*.hpp
    src/*.h
)

add_executable(RaylibEngineApp ${ENGINE_SOURCES} ${IMGUI_SOURCES})

target_include_directories(RaylibEngineApp PRIVATE
    src
    src/rlImGui
    ${imgui_SOURCE_DIR}
)


target_link_libraries(RaylibEngineApp PRIVATE
    raylib
    EnTT::EnTT
    nlohmann_json::nlohmann_json
)
