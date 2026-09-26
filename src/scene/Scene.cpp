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

void Scene::Clear() {
    m_registry.clear();
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
