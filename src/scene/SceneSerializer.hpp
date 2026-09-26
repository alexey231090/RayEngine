#pragma once

#include <string>
#include "Scene.hpp"

namespace REngine {

class SceneSerializer {
public:
    SceneSerializer(Scene& scene);

    bool Serialize(const std::string& filepath);
    bool Deserialize(const std::string& filepath);

    std::string SerializeToString();
    bool DeserializeFromString(const std::string& jsonString);

private:
    Scene& m_scene;
};

} // namespace REngine
