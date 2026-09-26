#include "SceneSerializer.hpp"
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace REngine {

SceneSerializer::SceneSerializer(Scene& scene) : m_scene(scene) {}

static std::string GeometryTypeToString(MeshGeometryType type) {
    switch (type) {
        case MeshGeometryType::Cube: return "Cube";
        case MeshGeometryType::Sphere: return "Sphere";
        case MeshGeometryType::Cylinder: return "Cylinder";
        case MeshGeometryType::Plane: return "Plane";
    }
    return "Cube";
}

static MeshGeometryType StringToGeometryType(const std::string& str) {
    if (str == "Sphere") return MeshGeometryType::Sphere;
    if (str == "Cylinder") return MeshGeometryType::Cylinder;
    if (str == "Plane") return MeshGeometryType::Plane;
    return MeshGeometryType::Cube;
}

static json BuildSceneJson(Scene& scene) {
    json rootJson;
    rootJson["scene"] = "REngineScene";
    rootJson["entities"] = json::array();

    auto& registry = scene.GetRegistry();
    auto view = registry.view<TagComponent>();

    for (auto entity : view) {
        json entityJson;
        const auto& tag = view.get<TagComponent>(entity);
        entityJson["name"] = tag.tag;

        if (registry.all_of<TransformComponent>(entity)) {
            const auto& t = registry.get<TransformComponent>(entity);
            entityJson["transform"] = {
                { "position", { t.position.x, t.position.y, t.position.z } },
                { "rotation", { t.rotation.x, t.rotation.y, t.rotation.z } },
                { "scale", { t.scale.x, t.scale.y, t.scale.z } }
            };
        }

        if (registry.all_of<MeshComponent>(entity)) {
            const auto& m = registry.get<MeshComponent>(entity);
            entityJson["mesh"] = {
                { "type", GeometryTypeToString(m.geometryType) },
                { "color", { m.color.r, m.color.g, m.color.b, m.color.a } },
                { "wireColor", { m.wireColor.r, m.wireColor.g, m.wireColor.b, m.wireColor.a } },
                { "drawWires", m.drawWires }
            };
        }

        if (registry.all_of<CameraComponent>(entity)) {
            const auto& c = registry.get<CameraComponent>(entity);
            entityJson["camera"] = {
                { "isPrimary", c.isPrimary },
                { "orbital", c.orbital },
                { "position", { c.camera.position.x, c.camera.position.y, c.camera.position.z } },
                { "target", { c.camera.target.x, c.camera.target.y, c.camera.target.z } },
                { "fovy", c.camera.fovy }
            };
        }

        rootJson["entities"].push_back(entityJson);
    }

    return rootJson;
}

static bool LoadSceneFromJson(Scene& scene, const json& rootJson) {
    scene.Clear();

    if (!rootJson.contains("entities") || !rootJson["entities"].is_array()) {
        return false;
    }

    for (const auto& entityJson : rootJson["entities"]) {
        std::string name = entityJson.value("name", "Entity");
        entt::entity entity = scene.CreateEntity(name);

        if (entityJson.contains("transform")) {
            auto& t = scene.GetRegistry().get_or_emplace<TransformComponent>(entity);
            const auto& tj = entityJson["transform"];
            if (tj.contains("position") && tj["position"].is_array() && tj["position"].size() >= 3) {
                t.position = { tj["position"][0], tj["position"][1], tj["position"][2] };
            }
            if (tj.contains("rotation") && tj["rotation"].is_array() && tj["rotation"].size() >= 3) {
                t.rotation = { tj["rotation"][0], tj["rotation"][1], tj["rotation"][2] };
            }
            if (tj.contains("scale") && tj["scale"].is_array() && tj["scale"].size() >= 3) {
                t.scale = { tj["scale"][0], tj["scale"][1], tj["scale"][2] };
            }
        }

        if (entityJson.contains("mesh")) {
            const auto& mj = entityJson["mesh"];
            MeshGeometryType gType = StringToGeometryType(mj.value("type", "Cube"));
            Color col = RED;
            Color wireCol = MAROON;
            bool drawWires = mj.value("drawWires", true);

            if (mj.contains("color") && mj["color"].is_array() && mj["color"].size() >= 4) {
                col = { (unsigned char)mj["color"][0], (unsigned char)mj["color"][1], (unsigned char)mj["color"][2], (unsigned char)mj["color"][3] };
            }
            if (mj.contains("wireColor") && mj["wireColor"].is_array() && mj["wireColor"].size() >= 4) {
                wireCol = { (unsigned char)mj["wireColor"][0], (unsigned char)mj["wireColor"][1], (unsigned char)mj["wireColor"][2], (unsigned char)mj["wireColor"][3] };
            }

            MeshComponent mesh(gType, col, wireCol);
            mesh.drawWires = drawWires;
            scene.GetRegistry().emplace<MeshComponent>(entity, mesh);
        }

        if (entityJson.contains("camera")) {
            const auto& cj = entityJson["camera"];
            CameraComponent cam;
            cam.isPrimary = cj.value("isPrimary", true);
            cam.orbital = cj.value("orbital", true);
            if (cj.contains("position") && cj["position"].is_array() && cj["position"].size() >= 3) {
                cam.camera.position = { cj["position"][0], cj["position"][1], cj["position"][2] };
            }
            if (cj.contains("target") && cj["target"].is_array() && cj["target"].size() >= 3) {
                cam.camera.target = { cj["target"][0], cj["target"][1], cj["target"][2] };
            }
            cam.camera.fovy = cj.value("fovy", 45.0f);
            scene.GetRegistry().emplace<CameraComponent>(entity, cam);
        }
    }

    return true;
}

bool SceneSerializer::Serialize(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << std::endl;
        return false;
    }

    json rootJson = BuildSceneJson(m_scene);
    file << rootJson.dump(2);
    return true;
}

std::string SceneSerializer::SerializeToString() {
    json rootJson = BuildSceneJson(m_scene);
    return rootJson.dump();
}

bool SceneSerializer::Deserialize(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open scene file for reading: " << filepath << std::endl;
        return false;
    }

    json rootJson;
    try {
        file >> rootJson;
    } catch (const std::exception& e) {
        std::cerr << "JSON Parse error: " << e.what() << std::endl;
        return false;
    }

    return LoadSceneFromJson(m_scene, rootJson);
}

bool SceneSerializer::DeserializeFromString(const std::string& jsonString) {
    try {
        json rootJson = json::parse(jsonString);
        return LoadSceneFromJson(m_scene, rootJson);
    } catch (const std::exception& e) {
        std::cerr << "JSON Parse string error: " << e.what() << std::endl;
        return false;
    }
}

} // namespace REngine
