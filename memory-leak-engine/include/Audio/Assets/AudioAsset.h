#pragma once

#include "Core/AssetManager/Asset.h"
#include "soloud.h"

namespace mlg {

    class AudioAsset : public Asset {
    private:
        std::unique_ptr<class Audio> audio;

    public:
        explicit AudioAsset(const std::string& path);
        ~AudioAsset() override;

        void Play(SoLoud::Soloud* soloud);
        void PlayBackgroundMusic(SoLoud::Soloud* soloud);

        void Load() override;
    };

}// namespace mlg
