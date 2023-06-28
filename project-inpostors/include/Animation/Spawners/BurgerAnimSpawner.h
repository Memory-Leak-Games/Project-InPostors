#pragma once

#include "AnimationSpawner.h"

class BurgerAnimSpawner : public AnimationSpawner {

public:
    BurgerAnimSpawner();

    void Spawn(const std::shared_ptr<mlg::Entity>& entity, const std::shared_ptr<mlg::MaterialAsset>& material) override;
};