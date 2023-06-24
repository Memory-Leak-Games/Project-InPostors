#pragma once

#include "AnimationSpawner.h"

class DonutAnimSpawner : public AnimationSpawner {

public:
    DonutAnimSpawner();

    void Spawn(const std::shared_ptr<mlg::Entity>& entity, const std::shared_ptr<mlg::MaterialAsset>& material) override;
};