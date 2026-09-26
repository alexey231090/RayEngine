#include "RenderSystem.hpp"
#include "raymath.h"
#include "rlgl.h"

namespace REngine {

void RenderSystem::DrawCameraGizmo(const Camera3D& cam) {
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

    // Camera body box
    DrawCubeWires(camPos, 0.5f, 0.4f, 0.6f, WHITE);
    DrawCube(camPos, 0.48f, 0.38f, 0.58f, (Color){ 60, 60, 70, 255 });

    // Frustum cone lines
    float frustumDist = 2.0f;
    float halfW = 1.2f;
    float halfH = 0.7f;
    Vector3 center = Vector3Add(camPos, Vector3Scale(forward, frustumDist));

    Vector3 c1 = Vector3Add(center, Vector3Add(Vector3Scale(right, halfW), Vector3Scale(up, halfH)));
    Vector3 c2 = Vector3Add(center, Vector3Add(Vector3Scale(right, -halfW), Vector3Scale(up, halfH)));
    Vector3 c3 = Vector3Add(center, Vector3Add(Vector3Scale(right, -halfW), Vector3Scale(up, -halfH)));
    Vector3 c4 = Vector3Add(center, Vector3Add(Vector3Scale(right, halfW), Vector3Scale(up, -halfH)));

    Color frustumCol = (Color){ 100, 200, 255, 200 };

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
    DrawLine3D(camPos, cam.target, (Color){ 255, 200, 0, 150 });
}

void RenderSystem::Render(Scene& scene, const Camera3D& camera, bool isEditMode) {
    BeginMode3D(camera);

    // Draw reference floor grid
    DrawGrid(20, 1.0f);

    // Render all ECS entities that have TransformComponent and MeshComponent
    auto& registry = scene.GetRegistry();
    auto view = registry.view<TransformComponent, MeshComponent>();

    for (auto entity : view) {
        const auto& transform = view.get<TransformComponent>(entity);
        const auto& mesh = view.get<MeshComponent>(entity);

        rlPushMatrix();
        rlTranslatef(transform.position.x, transform.position.y, transform.position.z);
        rlRotatef(transform.rotation.z, 0.0f, 0.0f, 1.0f);
        rlRotatef(transform.rotation.y, 0.0f, 1.0f, 0.0f);
        rlRotatef(transform.rotation.x, 1.0f, 0.0f, 0.0f);

        Vector3 originPos = { 0.0f, 0.0f, 0.0f };

        switch (mesh.geometryType) {
            case MeshGeometryType::Cube: {
                DrawCube(originPos, transform.scale.x, transform.scale.y, transform.scale.z, mesh.color);
                if (mesh.drawWires) {
                    DrawCubeWires(originPos, transform.scale.x, transform.scale.y, transform.scale.z, mesh.wireColor);
                }
                break;
            }
            case MeshGeometryType::Sphere: {
                float radius = transform.scale.x * 0.5f;
                DrawSphere(originPos, radius, mesh.color);
                if (mesh.drawWires) {
                    DrawSphereWires(originPos, radius, 16, 16, mesh.wireColor);
                }
                break;
            }
            case MeshGeometryType::Cylinder: {
                float radius = transform.scale.x * 0.5f;
                DrawCylinder(originPos, radius, radius, transform.scale.y, 16, mesh.color);
                if (mesh.drawWires) {
                    DrawCylinderWires(originPos, radius, radius, transform.scale.y, 16, mesh.wireColor);
                }
                break;
            }
            case MeshGeometryType::Plane: {
                DrawPlane(originPos, (Vector2){ transform.scale.x, transform.scale.z }, mesh.color);
                break;
            }
        }

        rlPopMatrix();
    }

    // In Edit mode, visualize all CameraComponents in the scene
    if (isEditMode) {
        auto camView = registry.view<CameraComponent>();
        for (auto camEntity : camView) {
            const auto& camComp = camView.get<CameraComponent>(camEntity);
            DrawCameraGizmo(camComp.camera);
        }
    }

    EndMode3D();
}

} // namespace REngine
