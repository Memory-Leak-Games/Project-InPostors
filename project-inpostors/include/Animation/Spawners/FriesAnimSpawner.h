#pragma once

#include "AnimationSpawner.h"

class FriesAnimSpawner : public AnimationSpawner {

public:
    FriesAnimSpawner();

    void Spawn(const std::shared_ptr<mlg::Entity>& entity, const std::shared_ptr<mlg::MaterialAsset>& material) override;
};