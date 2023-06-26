#include "Audio/Assets/AudioAsset.h"

#include "Audio/AudioAPI.h"

#include "Macros.h"
#include <cstdint>

namespace mlg {
    AudioAsset::AudioAsset(const std::string& path) : Asset(path) {
    }

    AudioAsset::~AudioAsset() = default;

    uint32_t AudioAsset::Play(float volume, float pan, uint32_t paused, uint32_t bus) {
        handle = mlg::AudioAPI::GetSoLoud()->play(sound, volume, pan, paused, bus);
        return handle;
    }

    uint32_t AudioAsset::PlayBackgroundMusic(float volume, uint32_t paused, uint32_t bus) {
        handle = mlg::AudioAPI::GetSoLoud()->playBackground(sound, volume, paused, bus);
        return handle;
    }

    uint32_t AudioAsset::PlayClocked(float delay, float volume, float pan, uint32_t bus) {
        handle = mlg::AudioAPI::GetSoLoud()->playClocked(delay, sound, volume, pan, bus);
        return handle;
    }

    void AudioAsset::Pause() const {
        Pause(handle);
    }

    void AudioAsset::Pause(uint32_t handle) const {
        mlg::AudioAPI::GetSoLoud()->setPause(handle, true);
    }

    void AudioAsset::UnPause() const {
        UnPause(handle);
    }

    void AudioAsset::UnPause(uint32_t handle) const {
        mlg::AudioAPI::GetSoLoud()->setPause(handle, false);
    }

    void AudioAsset::Seek(float seconds) {
        Seek(handle, seconds);
    }

    void AudioAsset::Seek(uint32_t handle, float seconds) {
        mlg::AudioAPI::GetSoLoud()->seek(handle, seconds);
    }

    void AudioAsset::Stop() {
        Stop(handle);
    }

    void AudioAsset::Stop(uint32_t handle) {
        mlg::AudioAPI::GetSoLoud()->stop(handle);
    }

    void AudioAsset::SetVolume(float volume) {
        SetVolume(handle, volume);
    }

    void AudioAsset::SetVolume(uint32_t handle, float volume) {
        mlg::AudioAPI::GetSoLoud()->setVolume(handle, volume);
    }

    void AudioAsset::SetLooping(bool isLooping) {
        sound.setLooping(isLooping);
    }

    void AudioAsset::SetSingleInstance() {
        sound.setSingleInstance(true);
    }

    void AudioAsset::SetFilter(uint32_t filterID, SoLoud::Filter* filter) {
        sound.setFilter(filterID, filter);
    }

    double AudioAsset::GetLength() {
        return sound.getLength();
    }

    void AudioAsset::Load() {
        SPDLOG_DEBUG("Loading sound Audio at path: {}", GetPath());
        sound.load(GetPath().c_str());
    }
}// namespace mlg