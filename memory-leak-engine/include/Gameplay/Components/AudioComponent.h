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

        void Play(SoLoud::Soloud* soloud, float volume = 1.f, float pan = 0.f,
                                 uint32_t paused = 0, uint32_t bus = 0);

        void PlayBackgroundMusic(SoLoud::Soloud* soloud, float volume = 1.f,
                                 uint32_t paused = 0, uint32_t bus = 0);

        void PlayClocked(SoLoud::Soloud* soloud, float delay, float volume = 1.f,
                         float pan = 0.f, uint32_t bus = 0);

        void Pause(SoLoud::Soloud* soloud);
        void UnPause(SoLoud::Soloud* soloud);

        void Seek(SoLoud::Soloud* soloud, float time);

        void SoundStop();
        void StopAll(SoLoud::Soloud* soloud);

        void SetVolume(SoLoud::Soloud* soloud, float volume);
        void SetLooping();
    };

}// namespace mlg
