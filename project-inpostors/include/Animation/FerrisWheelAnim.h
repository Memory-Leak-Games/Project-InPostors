#pragma once

namespace mlg {
    class Entity;
    class MaterialAsset;
}

class FerrisWheelAnim {

public:
    FerrisWheelAnim();

    void Spawn(const std::shared_ptr<mlg::Entity>& entity, const std::shared_ptr<mlg::MaterialAsset>& material);

    ~FerrisWheelAnim() = default;
};
