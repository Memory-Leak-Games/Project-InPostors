#include "Audio/Assets/SoundAsset.h"

#include "Audio/Audio.h"

#include "Macros.h"

namespace mlg {
    SoundAsset::SoundAsset(const std::string& path) : Asset(path) {
    }

    SoundAsset::~SoundAsset() {
    }

    void SoundAsset::Play(SoLoud::Soloud* soloud) {
        audio->Play(soloud);
    }

    void SoundAsset::PlayBackgroundMusic(SoLoud::Soloud* soloud) {
        audio->PlayBackgroundMusic(soloud);
    }

    void SoundAsset::Load() {
        SPDLOG_DEBUG("Loading sound Audio at path: {}", GetPath());
        audio = std::make_unique<Audio>(GetPath());
    }
}// namespace mlg