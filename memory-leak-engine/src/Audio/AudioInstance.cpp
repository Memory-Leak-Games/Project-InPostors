#include "Audio/AudioInstance.h"

#include "Audio/Assets/AudioAsset.h"

mlg::AudioInstance::AudioInstance(const std::shared_ptr<class AudioAsset>& audioAsset)
    : audioAsset(audioAsset) {}

mlg::AudioInstance::AudioInstance(const std::string& path) {
    audioAsset = mlg::AssetManager::GetAsset<AudioAsset>(path);
}

void mlg::AudioInstance::Play(float volume,
                              float pan,
                              uint32_t paused,
                              uint32_t bus) {
    if (handle == 0) {
        handle = audioAsset->Play(volume, pan, paused, bus);
        SPDLOG_DEBUG("Sound {} started", handle);
        return;
    }

    audioAsset->UnPause();
}

void mlg::AudioInstance::PlayBackgroundMusic(float volume,
                                             uint32_t paused,
                                             uint32_t bus) {
    if (handle == 0) {
        handle = audioAsset->PlayBackgroundMusic(volume, paused, bus);
        return;
    }

    audioAsset->UnPause();
}

void mlg::AudioInstance::PlayClocked(float delay,
                                     float volume,
                                     float pan,
                                     uint32_t bus) {
    if (handle == 0) {
        handle = audioAsset->PlayClocked(delay, volume, pan, bus);
        return;
    }

    audioAsset->UnPause();
}

void mlg::AudioInstance::Pause() const {
    audioAsset->Pause(handle);
}

void mlg::AudioInstance::UnPause() const {
    audioAsset->UnPause(handle);
}

void mlg::AudioInstance::Seek(float seconds) {
    audioAsset->Seek(handle, seconds);
}

void mlg::AudioInstance::Stop() {
    if (handle == 0) {
        return;
    }

    audioAsset->Stop(handle);

    SPDLOG_DEBUG("Sound {} stopped", handle);

    handle = 0;
}

void mlg::AudioInstance::SetVolume(float volume) {
    audioAsset->SetVolume(handle, volume);
}

void mlg::AudioInstance::SetLooping() {
    audioAsset->SetLooping(handle);
}

double mlg::AudioInstance::GetLength() {
    return audioAsset->GetLength();
}
