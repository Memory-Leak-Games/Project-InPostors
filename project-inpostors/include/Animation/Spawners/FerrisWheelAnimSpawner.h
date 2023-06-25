#pragma once

#include "AnimationSpawner.h"

class FerrisWheelAnimSpawner : public AnimationSpawner {

public:
    FerrisWheelAnimSpawner();

    void Spawn(const std::shared_ptr<mlg::Entity>& entity, const std::shared_ptr<mlg::MaterialAsset>& material) override;
};
