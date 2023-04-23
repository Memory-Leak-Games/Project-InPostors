#include "include/Gameplay/Components/AudioComponent.h"

#include "audio/Assets/AudioAsset.h"

namespace mlg {
    AudioComponent::AudioComponent(const std::weak_ptr<Entity>& owner, const std::string& name, const std::shared_ptr<AudioAsset>& audio) : Component(owner, name), audio(audio) {
    }

    void AudioComponent::Play(SoLoud::Soloud* soloud, float volume, float pan, uint32_t paused, uint32_t bus) {
        audio->Play(soloud, volume, pan, paused, bus);
    }

    void AudioComponent::PlayBackgroundMusic(SoLoud::Soloud* soloud, float volume, uint32_t paused, uint32_t bus) {
        audio->PlayBackgroundMusic(soloud, volume, paused, bus);
    }

    void AudioComponent::PlayClocked(SoLoud::Soloud* soloud, float delay, float volume, float pan, uint32_t bus) {
        audio->PlayClocked(soloud, delay, volume, pan, bus);
    }

    void AudioComponent::Pause(SoLoud::Soloud* soloud) {
        audio->Pause(soloud);
    }

    void AudioComponent::UnPause(SoLoud::Soloud* soloud) {
        audio->UnPause(soloud);
    }

    void AudioComponent::Seek(SoLoud::Soloud* soloud, float time) {
        audio->Seek(soloud, time);
    }

    void AudioComponent::SoundStop() {
        audio->Stop();
    }

    void AudioComponent::StopAll(SoLoud::Soloud* soloud) {
        audio->StopAll(soloud);
    }

    void AudioComponent::SetVolume(SoLoud::Soloud* soloud, float volume) {
        audio->SetVolume(soloud, volume);
    }

    void AudioComponent::SetLooping() {
        audio->SetLooping();
    }
}// namespace mlg