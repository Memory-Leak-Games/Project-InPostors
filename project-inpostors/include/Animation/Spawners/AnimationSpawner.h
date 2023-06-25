#pragma once

namespace mlg {
    class Entity;
    class MaterialAsset;
}

class AnimationSpawner {
public:
    virtual void Spawn(const std::shared_ptr<mlg::Entity>& entity, const std::shared_ptr<mlg::MaterialAsset>& material) = 0;
};
