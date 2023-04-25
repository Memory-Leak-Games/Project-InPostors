#pragma once

#include "Gameplay/SceneComponent.h"
#include "Rendering/Camera.h"
#include "Rendering/Renderable.h"

//TODO: Create abstract rendering camera;
namespace mlg {

    class CameraComponent : public Component {
    private:
        std::unique_ptr<Camera> camera;

    public:
        CameraComponent() = delete;
        CameraComponent(const std::weak_ptr<Entity>& owner, const std::string& name);

        void SetOrtho(float size, float nearPlane, float farPlane);
        void SetPerspective(float fov, float nearPlane, float farPlane);

        void SetActive();
        bool IsActive();

        void Update() override;

        Transform& GetTransform();

#ifdef DEBUG
        void UpdateImGUI();
#endif
    private:
    };

} // mlg
