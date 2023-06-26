#pragma once

#include "Gameplay/Component.h"
#include "soloud.h"

namespace mlg {

    class AudioComponent : public Component {
    private:
        std::shared_ptr<class AudioAsset> audio;

    public:
        AudioComponent(const std::weak_ptr<Entity>& owner, const std::string& name,
                       const std::shared_ptr<AudioAsset>& audio);

        void Play(float volume = 1.f, float pan = 0.f,
                                 uint32_t paused = 0, uint32_t bus = 0);

        void PlayBackgroundMusic(float volume = 1.f,
                                 uint32_t paused = 0, uint32_t bus = 0);

        void PlayClocked(float delay, float volume = 1.f,
                         float pan = 0.f, uint32_t bus = 0);

        void Pause();
        void UnPause();

        void Seek(float time);

        void SoundStop();
        void StopAll();

        void SetVolume(float volume);
        void SetLooping(bool isLooping);
    };

}// namespace mlg
