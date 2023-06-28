#pragma once

#include "AnimationSpawner.h"

class CaffeeAnimSpawner : public AnimationSpawner {

public:
    CaffeeAnimSpawner();

    void Spawn(const std::shared_ptr<mlg::Entity>& entity, const std::shared_ptr<mlg::MaterialAsset>& material) override;
};