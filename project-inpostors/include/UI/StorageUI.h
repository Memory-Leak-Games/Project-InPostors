#pragma once

#include "Gameplay/SceneComponent.h"

namespace mlg {
    class Image;
}

class StorageUI : public mlg::SceneComponent {
    std::shared_ptr<mlg::Image> pin;

    bool animate = false;
    float time = 0.f;
public:
    StorageUI(const std::weak_ptr<mlg::Entity>& owner, const std::string& name);

    void Start() override;
    void Update() override;
};
