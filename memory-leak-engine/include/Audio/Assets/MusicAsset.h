#pragma once

#include "Core/AssetManager/Asset.h"
#include "soloud.h"

namespace mlg {

    class MusicAsset : public Asset {
    private:
        std::unique_ptr<class MusicAudio> audio;

    public:
        explicit MusicAsset(const std::string& path);
        ~MusicAsset() override;

        void Play(SoLoud::Soloud* soloud);
        void PlayBackgroundMusic(SoLoud::Soloud* soloud);

        void Load() override;
    };

}// namespace mlg
