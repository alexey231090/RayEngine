#pragma once

#include "raylib.h"

namespace REngine {

enum class GizmoAxis {
    None,
    X,
    Y,
    Z
};

class Gizmo {
public:
    Gizmo();
    ~Gizmo() = default;

    // Returns true if gizmo is actively being dragged
    bool UpdateAndRender(const Camera3D& camera, Vector3& position, bool allowInteraction);

    bool IsDragging() const { return m_isDragging; }

private:
    GizmoAxis CheckHover(const Camera3D& camera, const Vector3& position);
    void DrawArrow(const Vector3& start, const Vector3& end, Color color, float cylinderRadius, float coneRadius, float coneLength);

private:
    GizmoAxis m_hoveredAxis = GizmoAxis::None;
    GizmoAxis m_activeAxis = GizmoAxis::None;
    bool m_isDragging = false;

    float m_arrowLength = 2.0f;
    float m_cylinderRadius = 0.05f;
    float m_coneRadius = 0.15f;
    float m_coneLength = 0.4f;
    float m_hitboxRadius = 0.25f;
};

} // namespace REngine
