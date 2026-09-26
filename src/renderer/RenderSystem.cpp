#include "RenderSystem.hpp"
#include "raymath.h"
#include "rlgl.h"

namespace REngine {

void RenderSystem::DrawCameraGizmo(const Camera3D& cam, bool isSelected) {
    Vector3 camPos = cam.position;
    Vector3 forward = Vector3Subtract(cam.target, cam.position);
    float dist = Vector3Length(forward);
    if (dist < 0.001f) forward = (Vector3){ 0.0f, 0.0f, -1.0f };
    else forward = Vector3Scale(forward, 1.0f / dist);

    Vector3 right = Vector3CrossProduct(forward, cam.up);
    float rightLen = Vector3Length(right);
    if (rightLen < 0.001f) right = (Vector3){ 1.0f, 0.0f, 0.0f };
    else right = Vector3Scale(right, 1.0f / rightLen);

    Vector3 up = Vector3Normalize(Vector3CrossProduct(right, forward));

    Color bodyWireCol = isSelected ? (Color){ 255, 230, 80, 255 } : WHITE;
    Color bodyCol = isSelected ? (Color){ 80, 80, 120, 220 } : (Color){ 60, 60, 70, 255 };

    // Camera body box
    DrawCubeWires(camPos, 0.5f, 0.4f, 0.6f, bodyWireCol);
    DrawCube(camPos, 0.48f, 0.38f, 0.58f, bodyCol);

    // Frustum cone lines
    float frustumDist = 2.0f;
    float halfW = 1.2f;
    float halfH = 0.7f;
    Vector3 center = Vector3Add(camPos, Vector3Scale(forward, frustumDist));

    Vector3 c1 = Vector3Add(center, Vector3Add(Vector3Scale(right, halfW), Vector3Scale(up, halfH)));
    Vector3 c2 = Vector3Add(center, Vector3Add(Vector3Scale(right, -halfW), Vector3Scale(up, halfH)));
    Vector3 c3 = Vector3Add(center, Vector3Add(Vector3Scale(right, -halfW), Vector3Scale(up, -halfH)));
    Vector3 c4 = Vector3Add(center, Vector3Add(Vector3Scale(right, halfW), Vector3Scale(up, -halfH)));

    Color frustumCol = isSelected ? (Color){ 255, 230, 80, 240 } : (Color){ 100, 200, 255, 200 };

    // Edges from camera to corners
    DrawLine3D(camPos, c1, frustumCol);
    DrawLine3D(camPos, c2, frustumCol);
    DrawLine3D(camPos, c3, frustumCol);
    DrawLine3D(camPos, c4, frustumCol);

    // Rectangle frame
    DrawLine3D(c1, c2, frustumCol);
    DrawLine3D(c2, c3, frustumCol);
    DrawLine3D(c3, c4, frustumCol);
    DrawLine3D(c4, c1, frustumCol);

    // Target direction line
    DrawLine3D(camPos, cam.target, (Color){ 255, 200, 0, isSelected ? (unsigned char)240 : (unsigned char)150 });
}

void RenderSystem::Render(Scene& scene, const Camera3D& camera, bool isEditMode, entt::entity selectedEntity) {
    BeginMode3D(camera);

    // Draw reference floor grid
    DrawGrid(20, 1.0f);

    // Render all ECS entities that have TransformComponent and MeshComponent
    auto& registry = scene.GetRegistry();
    auto view = registry.view<TransformComponent, MeshComponent>();

    auto drawEntityMesh = [&](entt::entity entity, bool isSelected) {
        const auto& transform = view.get<TransformComponent>(entity);
        const auto& mesh = view.get<MeshComponent>(entity);

        // When selected, make mesh translucent (alpha 0.35) and show vibrant selection wireframe
        Color meshColor = isSelected ? ColorAlpha(mesh.color, 0.35f) : mesh.color;
        Color wireColor = isSelected ? (Color){ 255, 230, 80, 255 } : mesh.wireColor;
        bool drawWires = isSelected || mesh.drawWires;

        rlPushMatrix();
        rlTranslatef(transform.position.x, transform.position.y, transform.position.z);
        rlRotatef(transform.rotation.z, 0.0f, 0.0f, 1.0f);
        rlRotatef(transform.rotation.y, 0.0f, 1.0f, 0.0f);
        rlRotatef(transform.rotation.x, 1.0f, 0.0f, 0.0f);

        Vector3 originPos = { 0.0f, 0.0f, 0.0f };

        switch (mesh.geometryType) {
            case MeshGeometryType::Cube: {
                DrawCube(originPos, transform.scale.x, transform.scale.y, transform.scale.z, meshColor);
                if (drawWires) {
                    DrawCubeWires(originPos, transform.scale.x, transform.scale.y, transform.scale.z, wireColor);
                }
                break;
            }
            case MeshGeometryType::Sphere: {
                float radius = transform.scale.x * 0.5f;
                DrawSphere(originPos, radius, meshColor);
                if (drawWires) {
                    DrawSphereWires(originPos, radius, 16, 16, wireColor);
                }
                break;
            }
            case MeshGeometryType::Cylinder: {
                float radius = transform.scale.x * 0.5f;
                DrawCylinder(originPos, radius, radius, transform.scale.y, 16, meshColor);
                if (drawWires) {
                    DrawCylinderWires(originPos, radius, radius, transform.scale.y, 16, wireColor);
                }
                break;
            }
            case MeshGeometryType::Plane: {
                DrawPlane(originPos, (Vector2){ transform.scale.x, transform.scale.z }, meshColor);
                if (drawWires) {
                    DrawCubeWires(originPos, transform.scale.x, 0.02f, transform.scale.z, wireColor);
                }
                break;
            }
            case MeshGeometryType::Capsule: {
                float radius = transform.scale.x * 0.5f;
                float totalHeight = transform.scale.y;
                float cylHeight = totalHeight - 2.0f * radius;
                if (cylHeight < 0.0f) cylHeight = 0.0f;

                float halfCyl = cylHeight * 0.5f;
                Vector3 topCenter = { originPos.x, originPos.y + halfCyl, originPos.z };
                Vector3 bottomCenter = { originPos.x, originPos.y - halfCyl, originPos.z };

                if (cylHeight > 0.001f) {
                    DrawCylinder(originPos, radius, radius, cylHeight, 16, meshColor);
                    if (drawWires) {
                        DrawCylinderWires(originPos, radius, radius, cylHeight, 16, wireColor);
                    }
                }

                DrawSphere(topCenter, radius, meshColor);
                DrawSphere(bottomCenter, radius, meshColor);

                if (drawWires) {
                    DrawSphereWires(topCenter, radius, 12, 12, wireColor);
                    DrawSphereWires(bottomCenter, radius, 12, 12, wireColor);
                }
                break;
            }
        }

        rlPopMatrix();
    };

    // Pass 1: Draw unselected (opaque) entities first
    for (auto entity : view) {
        if (!isEditMode || entity != selectedEntity) {
            drawEntityMesh(entity, false);
        }
    }

    // Pass 2: Draw selected entity (translucent with wireframe outline) after opaque meshes for proper blending
    if (isEditMode && selectedEntity != entt::null && registry.valid(selectedEntity) && view.contains(selectedEntity)) {
        drawEntityMesh(selectedEntity, true);
    }

    // In Edit mode, visualize all CameraComponents in the scene
    if (isEditMode) {
        auto camView = registry.view<CameraComponent>();
        for (auto camEntity : camView) {
            const auto& camComp = camView.get<CameraComponent>(camEntity);
            bool isCamSelected = (camEntity == selectedEntity);
            DrawCameraGizmo(camComp.camera, isCamSelected);
        }
    }

    EndMode3D();
}

} // namespace REngine
