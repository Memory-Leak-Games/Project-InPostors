#include "Audio/Assets/AudioAsset.h"

#include "Audio/Audio.h"

#include "Macros.h"

namespace mlg {
    AudioAsset::AudioAsset(const std::string& path) : Asset(path) {
    }

    AudioAsset::~AudioAsset() = default;

    void AudioAsset::Play(float volume, float pan, uint32_t paused, uint32_t bus) {
        audio->Play(volume, pan, paused, bus);
    }

    void AudioAsset::PlayBackgroundMusic(float volume, uint32_t paused, uint32_t bus) {
        audio->PlayBackgroundMusic(volume, paused, bus);
    }

    void AudioAsset::PlayClocked(float delay, float volume, float pan, uint32_t bus) {
        audio->PlayClocked(delay, volume, pan, bus);
    }

    void AudioAsset::Pause() const {
        audio->Pause();
    }

    void AudioAsset::UnPause() const {
        audio->UnPause();
    }

    void AudioAsset::Seek(float time) {
        audio->Seek(time);
    }

    void AudioAsset::Stop() {
        audio->Stop();
    }

    void AudioAsset::StopAll() {
        audio->StopAll();
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