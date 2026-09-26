#include "Layer.hpp"
#include "Application.hpp"

namespace REngine {

Application& Layer::GetApp() {
    return Application::Get();
}

Scene& Layer::GetScene() {
    return Application::Get().GetScene();
}

Camera3D Layer::GetPrimaryCamera() {
    return Application::Get().GetPrimaryCamera();
}

void Layer::SetPrimaryCamera(const Camera3D& camera) {
    Application::Get().SetPrimaryCamera(camera);
}

} // namespace REngine
