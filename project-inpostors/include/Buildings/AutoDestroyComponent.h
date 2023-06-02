#pragma once

#include "Gameplay/Component.h"
#include <cstddef>

namespace mlg {
    class Entity;
}

class AutoDestroyComponent : public mlg::Component {
private:
    float timeToDestroy;
    size_t timerHandle;

public:
    explicit AutoDestroyComponent(
            std::weak_ptr<mlg::Entity> owner,
            std::string name,
            float timeToDestory);

    void Start() override;
};