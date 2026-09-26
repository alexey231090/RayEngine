#pragma once

#include <string>
#include "raylib.h"

namespace REngine {

struct TagComponent {
    std::string tag = "Entity";

    TagComponent() = default;
    TagComponent(const std::string& tag) : tag(tag) {}
};

struct TransformComponent {
    Vector3 position = { 0.0f, 0.0f, 0.0f };
    Vector3 rotation = { 0.0f, 0.0f, 0.0f }; // Euler angles in degrees
    Vector3 scale = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(const Vector3& pos) : position(pos) {}
};

enum class MeshGeometryType {
    Cube = 0,
    Sphere = 1,
    Cylinder = 2,
    Plane = 3,
    Capsule = 4
};

struct MeshComponent {
    MeshGeometryType geometryType = MeshGeometryType::Cube;
    Color color = RED;
    Color wireColor = MAROON;
    bool drawWires = true;

    MeshComponent() = default;
    MeshComponent(MeshGeometryType type, Color col, Color wire = MAROON)
        : geometryType(type), color(col), wireColor(wire), drawWires(true) {}
};

struct CameraComponent {
    Camera3D camera = { 0 };
    bool isPrimary = true;
    bool orbital = true;

    CameraComponent() {
        camera.position = (Vector3){ 0.0f, 10.0f, 10.0f };
        camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        camera.fovy = 45.0f;
        camera.projection = CAMERA_PERSPECTIVE;
    }
};

} // namespace REngine
