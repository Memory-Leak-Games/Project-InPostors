#pragma once

#include "Core/AssetManager/Asset.h"

namespace mlg {

    class ModelAsset : public Asset {
    private:
        std::unique_ptr<class Model> model;

    public:
        explicit ModelAsset(const std::string& path);
        ~ModelAsset() override;

        void Draw();

        void Load() override;
    };

} // mlg
