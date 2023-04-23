#include "include/Gameplay/Components/AudioComponent.h"

#include "audio/Assets/AudioAsset.h"

namespace mlg {
    AudioComponent::AudioComponent(const std::weak_ptr<Entity>& owner, const std::string& name, const std::shared_ptr<AudioAsset>& audio) : Component(owner, name), audio(audio) {
    }

    void AudioComponent::Play(SoLoud::Soloud* soloud) {
        audio->Play(soloud);
    }
}// namespace mlg