#include "GameplayLayer/Nodes/CameraNode.h"
#include "Core/CoreEngine.h"
#include "LowLevelRenderer/Camera.h"

using namespace mlg;

CameraNode::CameraNode() {
    camera = Camera::GetInstance();
}

void CameraNode::Update(float seconds, float deltaSeconds) {
    Node::Update(seconds, deltaSeconds);

    CoreEngine* engine = CoreEngine::GetInstance();

    if (engine->currentCamera != this)
        return;

    Window::Resolution resolution = Window::GetInstance()->GetResolution();
    glm::vec<2, int> currentResolution{resolution.width, resolution. height};

    camera->SetResolution(currentResolution);

    camera->SetPosition(GetWorldPosition());
    camera->SetRotation(GetForwardVector(), GetUpVector());
}

void CameraNode::SetActive() {
    CoreEngine* engine = CoreEngine::GetInstance();
    engine->currentCamera = this;
}