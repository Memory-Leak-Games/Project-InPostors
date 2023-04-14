#pragma once

#include "Gameplay/SceneComponent.h"

namespace mlg {

    class CameraComponent : public SceneComponent {
    private:
        CameraComponent() = delete;

    public:
        CameraComponent(const std::weak_ptr<Entity>& owner, const std::string& name);

        void SetOrtho(float size, float near, float far);
        void SetPerspective(float fov, float near, float far);

        void Update() override;

#ifdef DEBUG
        void UpdateImGUI();
#endif
    };

} // mlg
