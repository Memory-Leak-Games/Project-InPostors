#include "Audio/Assets/AudioAsset.h"

#include "Audio/Audio.h"

#include "Macros.h"

namespace mlg {
    AudioAsset::AudioAsset(const std::string& path) : Asset(path) {
    }

    AudioAsset::~AudioAsset() {
    }

    void AudioAsset::Play(SoLoud::Soloud* soloud) {
        audio->Play(soloud);
    }

    void AudioAsset::PlayBackgroundMusic(SoLoud::Soloud* soloud) {
        audio->PlayBackgroundMusic(soloud);
    }

    void AudioAsset::Load() {
        SPDLOG_DEBUG("Loading sound Audio at path: {}", GetPath());
        audio = std::make_unique<Audio>(GetPath());
    }
}// namespace mlg