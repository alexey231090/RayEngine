#include "Gizmo.hpp"
#include "raymath.h"
#include "rlgl.h"

namespace REngine {

Gizmo::Gizmo() {}

void Gizmo::DrawArrow(const Vector3& start, const Vector3& end, Color color, float cylinderRadius, float coneRadius, float coneLength) {
    Vector3 dir = Vector3Normalize(Vector3Subtract(end, start));
    float totalLength = Vector3Distance(start, end);
    float shaftLength = totalLength - coneLength;
    if (shaftLength < 0.1f) shaftLength = 0.1f;

    Vector3 coneBase = Vector3Add(start, Vector3Scale(dir, shaftLength));

    // Shaft
    DrawCylinderEx(start, coneBase, cylinderRadius, cylinderRadius, 12, color);
    // Tip cone
    DrawCylinderEx(coneBase, end, coneRadius, 0.0f, 16, color);
}

GizmoAxis Gizmo::CheckHover(const Camera3D& camera, const Vector3& position) {
    Ray mouseRay = GetMouseRay(GetMousePosition(), camera);

    float r = m_hitboxRadius;
    float len = m_arrowLength;

    // Axis X Bounding Box
    BoundingBox boxX = {
        { position.x, position.y - r, position.z - r },
        { position.x + len, position.y + r, position.z + r }
    };
    // Axis Y Bounding Box
    BoundingBox boxY = {
        { position.x - r, position.y, position.z - r },
        { position.x + r, position.y + len, position.z + r }
    };
    // Axis Z Bounding Box
    BoundingBox boxZ = {
        { position.x - r, position.y - r, position.z },
        { position.x + r, position.y + r, position.z + len }
    };

    RayCollision colX = GetRayCollisionBox(mouseRay, boxX);
    RayCollision colY = GetRayCollisionBox(mouseRay, boxY);
    RayCollision colZ = GetRayCollisionBox(mouseRay, boxZ);

    float closestDist = 999999.0f;
    GizmoAxis hitAxis = GizmoAxis::None;

    if (colX.hit && colX.distance < closestDist) {
        closestDist = colX.distance;
        hitAxis = GizmoAxis::X;
    }
    if (colY.hit && colY.distance < closestDist) {
        closestDist = colY.distance;
        hitAxis = GizmoAxis::Y;
    }
    if (colZ.hit && colZ.distance < closestDist) {
        closestDist = colZ.distance;
        hitAxis = GizmoAxis::Z;
    }

    return hitAxis;
}

bool Gizmo::UpdateAndRender(const Camera3D& camera, Vector3& position, bool allowInteraction) {
    if (allowInteraction) {
        if (!m_isDragging) {
            m_hoveredAxis = CheckHover(camera, position);
            if (m_hoveredAxis != GizmoAxis::None && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                m_isDragging = true;
                m_activeAxis = m_hoveredAxis;
            }
        } else {
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                m_isDragging = false;
                m_activeAxis = GizmoAxis::None;
            } else {
                Vector3 axisDir = { 0.0f, 0.0f, 0.0f };
                if (m_activeAxis == GizmoAxis::X) axisDir.x = 1.0f;
                if (m_activeAxis == GizmoAxis::Y) axisDir.y = 1.0f;
                if (m_activeAxis == GizmoAxis::Z) axisDir.z = 1.0f;

                Vector2 screenStart = GetWorldToScreen(position, camera);
                Vector2 screenEnd = GetWorldToScreen(Vector3Add(position, Vector3Scale(axisDir, m_arrowLength)), camera);
                Vector2 screenAxis = Vector2Subtract(screenEnd, screenStart);
                float screenLenSq = Vector2LengthSqr(screenAxis);

                if (screenLenSq > 1.0f) {
                    Vector2 mouseDelta = GetMouseDelta();
                    float dot = Vector2DotProduct(mouseDelta, screenAxis);
                    float dragFactor = (dot / screenLenSq) * m_arrowLength;
                    position = Vector3Add(position, Vector3Scale(axisDir, dragFactor));
                }
            }
        }
    } else {
        if (!m_isDragging) {
            m_hoveredAxis = GizmoAxis::None;
        }
    }

    // Render Gizmo arrows
    Vector3 endX = Vector3Add(position, (Vector3){ m_arrowLength, 0.0f, 0.0f });
    Vector3 endY = Vector3Add(position, (Vector3){ 0.0f, m_arrowLength, 0.0f });
    Vector3 endZ = Vector3Add(position, (Vector3){ 0.0f, 0.0f, m_arrowLength });

    Color colX = (m_hoveredAxis == GizmoAxis::X || m_activeAxis == GizmoAxis::X) ? YELLOW : RED;
    Color colY = (m_hoveredAxis == GizmoAxis::Y || m_activeAxis == GizmoAxis::Y) ? YELLOW : LIME;
    Color colZ = (m_hoveredAxis == GizmoAxis::Z || m_activeAxis == GizmoAxis::Z) ? YELLOW : (Color){ 0, 150, 255, 255 };

    // Draw Gizmo arrows with depth test disabled and batch flushed so they are ALWAYS visible
    rlDrawRenderBatchActive();
    rlDisableDepthTest();

    // Draw central origin sphere
    DrawSphere(position, m_cylinderRadius * 1.8f, WHITE);

    // Draw X, Y, Z arrows
    DrawArrow(position, endX, colX, m_cylinderRadius, m_coneRadius, m_coneLength);
    DrawArrow(position, endY, colY, m_cylinderRadius, m_coneRadius, m_coneLength);
    DrawArrow(position, endZ, colZ, m_cylinderRadius, m_coneRadius, m_coneLength);

    rlDrawRenderBatchActive();
    rlEnableDepthTest();

    return m_isDragging;
}

} // namespace REngine
