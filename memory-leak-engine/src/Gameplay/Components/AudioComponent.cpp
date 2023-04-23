#include "include/Gameplay/Components/AudioComponent.h"

#include "Audio/Assets/AudioAsset.h" 

namespace mlg {
    AudioComponent::AudioComponent(const std::weak_ptr<Entity>& owner, const std::string& name, const std::shared_ptr<AudioAsset>& audio) : Component(owner, name), audio(audio) {
    }

    void AudioComponent::Play(float volume, float pan, uint32_t paused, uint32_t bus) {
        audio->Play(volume, pan, paused, bus);
    }

    void AudioComponent::PlayBackgroundMusic(float volume, uint32_t paused, uint32_t bus) {
        audio->PlayBackgroundMusic(volume, paused, bus);
    }

    void AudioComponent::PlayClocked(float delay, float volume, float pan, uint32_t bus) {
        audio->PlayClocked(delay, volume, pan, bus);
    }

    void AudioComponent::Pause() {
        audio->Pause();
    }

    void AudioComponent::UnPause() {
        audio->UnPause();
    }

    void AudioComponent::Seek(float time) {
        audio->Seek(time);
    }

    void AudioComponent::SoundStop() {
        audio->Stop();
    }

    void AudioComponent::StopAll() {
        audio->StopAll();
    }

    void AudioComponent::SetVolume(float volume) {
        audio->SetVolume(volume);
    }

    void AudioComponent::SetLooping() {
        audio->SetLooping();
    }
}// namespace mlg