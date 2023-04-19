#include "Audio/Assets/AudioAsset.h"

#include "Audio/Audio.h"

#include "Macros.h"

namespace mlg {
    AudioAsset::AudioAsset(const std::string& path) : Asset(path) {
    }

    AudioAsset::~AudioAsset() {
    }

    void AudioAsset::Play(SoLoud::Soloud* soloud, float volume, float pan, uint32_t paused, uint32_t bus) {
        audio->Play(soloud, volume, pan, paused, bus);
    }

    void AudioAsset::PlayBackgroundMusic(SoLoud::Soloud* soloud, float volume, uint32_t paused, uint32_t bus) {
        audio->PlayBackgroundMusic(soloud, volume, paused, bus);
    }

    void AudioAsset::PlayClocked(SoLoud::Soloud* soloud, float delay, float volume, float pan, uint32_t bus) {
        audio->PlayClocked(soloud, delay, volume, pan, bus);
    }

    void AudioAsset::Pause(SoLoud::Soloud* soloud) const {
        audio->Pause(soloud);
    }

    void AudioAsset::UnPause(SoLoud::Soloud* soloud) const {
        audio->UnPause(soloud);
    }

    void AudioAsset::Seek(SoLoud::Soloud* soloud, float time) {
        audio->Seek(soloud, time);
    }

    void AudioAsset::Stop() {
        audio->Stop();
    }

    void AudioAsset::StopAll(SoLoud::Soloud* soloud) {
        audio->StopAll(soloud);
    }

    void AudioAsset::SetVolume(float volume) {
        audio->SetVolume(volume);
    }

    void AudioAsset::SetLooping() {
        audio->SetLooping();
    }

    void AudioAsset::SetSingleInstance() {
        audio->SetSingleInstance();
    }

    void AudioAsset::SetFilter(uint32_t filterID, SoLoud::Filter* filter) {
        audio->SetFilter(filterID, filter);
    }

    double AudioAsset::GetLength() {
        return audio->GetLength();
    }

    void AudioAsset::Load() {
        SPDLOG_DEBUG("Loading sound Audio at path: {}", GetPath());
        audio = std::make_unique<Audio>(GetPath());
    }
}// namespace mlg