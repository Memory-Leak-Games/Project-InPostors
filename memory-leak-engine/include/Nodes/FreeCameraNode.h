#pragma once

#include "CameraNode.h"

namespace mlg {
    class FreeCameraNode : public CameraNode {
    private:
        const float speed = 30.f;
        const float acceleration = 15.f;
        const float mouseSensitivity = 0.18;

        glm::vec3 velocity;
    public:
        explicit FreeCameraNode(class CoreEngine *engine);

        void Update(struct CoreEngine *engine, float seconds, float deltaSeconds) override;

    private:
        void HandleMovement(float deltaSeconds);

        void HandleRotation();

        glm::vec3 GetMovementInput();
    };
}
