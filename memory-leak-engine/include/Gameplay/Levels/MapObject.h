#pragma once

namespace mlg {
    struct MapObject {
        std::shared_ptr<class ModelAsset> model;
        std::shared_ptr<class MaterialAsset> material;

        float worldRot;
        float scale;

        bool hasCollision = false;
        std::string colliderType;
        float colliderSize = 1.0f;
        float colliderOffset = 0.0f;
    };
} // mlg

