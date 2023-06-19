#include "Gameplay/Components/CameraComponent.h"

#include "Events/WindowEvent.h"

#include "Gameplay/Entity.h"

namespace mlg {

    CameraComponent::CameraComponent(const std::weak_ptr<Entity>& owner, const std::string& name)
        : Component(owner, name), camera(std::make_unique<Camera>()) {
        owner.lock()->GetTransform().AddChild(camera->transform);
    }

    void CameraComponent::Update() {
        camera->Update();
#ifdef DEBUG
        UpdateImGUI();
#endif
    }

    void CameraComponent::SetOrtho(float size, float nearPlane, float farPlane) {
        camera->SetOrtho(size, nearPlane, farPlane);
    }

    void CameraComponent::SetPerspective(float fov, float nearPlane, float farPlane) {
        camera->SetPerspective(fov, nearPlane, farPlane);
    }

    void CameraComponent::SetActive() {
        camera->SetActive();
    }

    bool CameraComponent::IsActive() {
        return camera->IsActive();
    }

    Transform& CameraComponent::GetTransform() {
        return camera->GetTransform();
    }


#ifdef DEBUG

    void CameraComponent::UpdateImGUI() {
        if (!camera->IsActive())
            return;

        ImGui::Begin("Camera");

        glm::vec3 position = camera->GetTransform().GetWorldPosition();
        glm::vec3 rotation = glm::degrees(camera->GetTransform().GetEulerRotation());

        bool isViewChanged = false;
        isViewChanged |= ImGui::DragFloat3("Camera Position", (float*) &position, 0.1f);
        isViewChanged |= ImGui::DragFloat3("Camera Rotation", (float*) &rotation, 1.f);

        if (isViewChanged) {
            camera->GetTransform().SetPosition(position);
            camera->GetTransform().SetRotation({glm::radians(rotation)});
        }

        auto orthoProjection = dynamic_cast<Camera::OrthoProjection*>(camera->GetProjection());
        auto perspectiveProjection = dynamic_cast<Camera::PerspectiveProjection*>(camera->GetProjection());

        if (orthoProjection) {
            float size = orthoProjection->size;
            if (ImGui::DragFloat("Ortho Size", &size, 0.1)) {
                camera->SetOrtho(size, orthoProjection->nearPlane, orthoProjection->farPlane);
            }
        } else if (perspectiveProjection) {
            float fow = perspectiveProjection->fov;
            if (ImGui::DragFloat("Fow", &fow, 0.1)) {
                camera->SetOrtho(fow, perspectiveProjection->nearPlane, perspectiveProjection->farPlane);
            }
        }

        ImGui::End();
    }

#endif

}// namespace mlg