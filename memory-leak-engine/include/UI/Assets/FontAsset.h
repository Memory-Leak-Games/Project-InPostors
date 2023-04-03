#pragma once

#include "Core/AssetManager/Asset.h"

namespace mlg {

    class FontAsset : Asset {
        explicit FontAsset(const std::string& path);

        ~FontAsset() override;
        void Load() override;
    };

}
