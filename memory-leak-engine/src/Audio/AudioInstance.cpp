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
    handle = audioAsset->Play(volume, pan, paused, bus);
}

void mlg::AudioInstance::PlayBackgroundMusic(float volume,
                                             uint32_t paused,
                                             uint32_t bus) {
    handle = audioAsset->PlayBackgroundMusic(volume, paused, bus);
}

void mlg::AudioInstance::PlayClocked(float delay,
                                     float volume,
                                     float pan,
                                     uint32_t bus) {
    handle = audioAsset->PlayClocked(delay, volume, pan, bus);
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
    handle = 0;
}

void mlg::AudioInstance::SetVolume(float volume) {
    audioAsset->SetVolume(handle, volume);
}

double mlg::AudioInstance::GetLength() {
    return audioAsset->GetLength();
}

std::shared_ptr<mlg::AudioAsset> mlg::AudioInstance::GetAudioAsset() const {
    return audioAsset;
}
