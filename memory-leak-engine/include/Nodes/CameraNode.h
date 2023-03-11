#pragma once

#include "Node.h"

namespace mlg {
    class CameraNode : public Node {
    private:
        std::shared_ptr<class Camera> camera;

    protected:
        class CoreEngine *engine;

    public:
        CameraNode(CoreEngine *engine);

        void Update(struct CoreEngine *engine, float seconds, float deltaSeconds) override;

        void SetActive();
    };
}