#include "Nodes/CameraNode.h"
#include "Camera.h"
#include "CoreEngine.h"

using namespace mlg;

CameraNode::CameraNode(CoreEngine *engine) : engine(engine) {
    camera = Camera::GetInstance();
}

void CameraNode::Update(struct CoreEngine *engine, float seconds, float deltaSeconds) {
    Node::Update(engine, seconds, deltaSeconds);

    if (engine->currentCamera != this)
        return;

    glm::vec<2, int> currentResolution{};
    glfwGetFramebufferSize(engine->GetWindow(), &currentResolution.x, &currentResolution.y);

    camera->SetResolution(currentResolution);

    camera->SetPosition(GetWorldPosition());
    camera->SetRotation(GetForwardVector(), GetUpVector());
}

void CameraNode::SetActive() {
    engine->currentCamera = this;
}
