#pragma once

#include "Node.h"

namespace mlg {
    class CameraNode : public Node {
    private:
        std::shared_ptr<class Camera> camera;

    public:
        CameraNode();

        void Update(float seconds, float deltaSeconds) override;

        void SetActive();
    };
}