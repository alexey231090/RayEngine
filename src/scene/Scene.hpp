#pragma once

#include <string>
#include <vector>
#include "entt/entt.hpp"
#include "Components.hpp"

namespace REngine {

class Scene {
public:
    Scene();
    ~Scene();

    entt::entity CreateEntity(const std::string& name = "Entity");
    void DestroyEntity(entt::entity entity);
    void Clear(bool keepPrimaryCamera = false);

    entt::registry& GetRegistry() { return m_registry; }
    const entt::registry& GetRegistry() const { return m_registry; }

    entt::entity GetPrimaryCameraEntity();

private:
    entt::registry m_registry;
};

} // namespace REngine
