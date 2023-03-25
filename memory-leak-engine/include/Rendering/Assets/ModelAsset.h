#pragma once

#include "Core/AssetManager/Asset.h"

namespace mlg {

    class ModelAsset : public Asset {
    private:
        std::unique_ptr<class Model> model;

    public:
        explicit ModelAsset(const std::string& path);

        void Draw();

        void Load() override;
    };

} // mlg
