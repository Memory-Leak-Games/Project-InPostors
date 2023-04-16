#pragma once

#include "Core/AssetManager/Asset.h"
#include "soloud.h"

namespace mlg {

    class SoundAsset : public Asset {
    private:
        std::unique_ptr<class Audio> audio;

    public:
        explicit SoundAsset(const std::string& path);
        ~SoundAsset() override;

        void Play(SoLoud::Soloud* soloud);

        void Load() override;
    };

}// namespace mlg
