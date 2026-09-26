#pragma once

#include <string>

namespace REngine {

class Layer {
public:
    Layer(const std::string& name = "Layer") : m_debugName(name) {}
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(float dt) {}
    virtual void OnRender3D() {}
    virtual void OnRenderUI() {}

    const std::string& GetName() const { return m_debugName; }

protected:
    std::string m_debugName;
};

} // namespace REngine
