#pragma once

#include "Core/AssetManager/Asset.h"

namespace mlg {

    class TextureAsset : public Asset {
        uint32_t textureID = 0;
    public:
        explicit TextureAsset(const std::string& path);

        void Bind();

        void Load() override;
        ~TextureAsset() override;
    };

}// namespace mlg
