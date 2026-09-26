#pragma once

#include "raylib.h"
#include "scene/Scene.hpp"

namespace REngine {

class RenderSystem {
public:
    RenderSystem() = default;
    ~RenderSystem() = default;

    void Render(Scene& scene, const Camera3D& camera, bool isEditMode = true);

private:
    void DrawCameraGizmo(const Camera3D& gameCamera);
};

} // namespace REngine
