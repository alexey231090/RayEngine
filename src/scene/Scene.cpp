#include "Scene.hpp"

namespace REngine {

Scene::Scene() {}

Scene::~Scene() {
    Clear();
}

entt::entity Scene::CreateEntity(const std::string& name) {
    entt::entity entity = m_registry.create();
    m_registry.emplace<TagComponent>(entity, name);
    m_registry.emplace<TransformComponent>(entity);
    return entity;
}

void Scene::DestroyEntity(entt::entity entity) {
    if (m_registry.valid(entity)) {
        m_registry.destroy(entity);
    }
}

void Scene::Clear(bool keepPrimaryCamera) {
    if (!keepPrimaryCamera) {
        m_registry.clear();
        return;
    }

    entt::entity primaryCam = GetPrimaryCameraEntity();
    auto view = m_registry.view<TagComponent>();
    std::vector<entt::entity> toDestroy;
    for (auto entity : view) {
        if (entity != primaryCam) {
            toDestroy.push_back(entity);
        }
    }
    for (auto entity : toDestroy) {
        m_registry.destroy(entity);
    }
}

entt::entity Scene::GetPrimaryCameraEntity() {
    auto view = m_registry.view<CameraComponent>();
    for (auto entity : view) {
        const auto& camera = view.get<CameraComponent>(entity);
        if (camera.isPrimary) {
            return entity;
        }
    }
    return entt::null;
}

} // namespace REngine
