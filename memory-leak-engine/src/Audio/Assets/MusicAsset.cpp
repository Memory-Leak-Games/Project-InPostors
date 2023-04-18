#include "Audio/Assets/MusicAsset.h"

#include "Audio/MusicAudio.h"

#include "Macros.h"

namespace mlg {
    MusicAsset::MusicAsset(const std::string& path) : Asset(path) {
    }

    MusicAsset::~MusicAsset() {
    }

    void MusicAsset::Play(SoLoud::Soloud* soloud) {
        audio->Play(soloud);
    }

    void MusicAsset::PlayBackgroundMusic(SoLoud::Soloud* soloud) {
        audio->PlayBackgroundMusic(soloud);
    }

    void MusicAsset::Load() {
        SPDLOG_DEBUG("Loading music Audio at path: {}", GetPath());
        audio = std::make_unique<MusicAudio>(GetPath());
    }
}// namespace mlg