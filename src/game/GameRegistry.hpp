#pragma once
#include "core/Layer.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace REngine {

class GameRegistry {
public:
    using LayerFactory = std::function<std::shared_ptr<Layer>()>;

    static GameRegistry& Get() {
        static GameRegistry instance;
        return instance;
    }

    template <typename T>
    void Register(const std::string& name) {
        m_factories[name] = []() -> std::shared_ptr<Layer> {
            return std::make_shared<T>();
        };
        m_registeredNames.push_back(name);
    }

    std::shared_ptr<Layer> Create(const std::string& name) {
        auto it = m_factories.find(name);
        if (it != m_factories.end()) {
            return it->second();
        }
        return nullptr;
    }

    std::shared_ptr<Layer> CreateDefault() {
        if (!m_registeredNames.empty()) {
            return Create(m_registeredNames.back());
        }
        return nullptr;
    }

    const std::vector<std::string>& GetRegisteredNames() const {
        return m_registeredNames;
    }

    bool HasLayer(const std::string& name) const {
        return m_factories.find(name) != m_factories.end();
    }

private:
    GameRegistry() = default;
    std::unordered_map<std::string, LayerFactory> m_factories;
    std::vector<std::string> m_registeredNames;
};

// Helper struct for static auto-registration
template <typename T>
struct LayerAutoRegister {
    LayerAutoRegister(const std::string& name) {
        GameRegistry::Get().Register<T>(name);
    }
};

#define REGISTER_GAME_LAYER(ClassType, NameStr) \
    static ::REngine::LayerAutoRegister<ClassType> _auto_reg_##ClassType(NameStr);

} // namespace REngine
