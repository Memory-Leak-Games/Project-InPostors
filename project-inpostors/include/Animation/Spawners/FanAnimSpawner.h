#pragma once

#include "AnimationSpawner.h"

class FanAnimSpawner : public AnimationSpawner {

public:
    FanAnimSpawner();

    void Spawn(const std::shared_ptr<mlg::Entity>& entity, const std::shared_ptr<mlg::MaterialAsset>& material) override;
};