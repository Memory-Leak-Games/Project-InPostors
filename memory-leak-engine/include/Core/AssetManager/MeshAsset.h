#pragma once

#include "Core/AssetManager/Asset.h"

namespace mlg {

    class MeshAsset : public Asset {
    public:
        void Load() override;
        ~MeshAsset() override;
    };

}// namespace mlg
