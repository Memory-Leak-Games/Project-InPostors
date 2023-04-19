#pragma once

#include "soloud_wav.h"

namespace mlg {

    class Audio {
        std::string audioPath;
        SoLoud::Wav sound;
        uint32_t handle;

    public:
        Audio(const std::string& path);
        ~Audio();

        // Normal play
        void Play(SoLoud::Soloud* soloud,
                  float volume = 1.f,
                  float pan = 0.f,
                  uint32_t paused = 0,
                  uint32_t bus = 0);

        // Play without panning
        void PlayBackgroundMusic(SoLoud::Soloud* soloud);

        // Play with delay
        void PlayClocked(SoLoud::Soloud* soloud, float delay);

        void Pause();

        void Stop();
        void StopAll();

        void SetVolume();
    };

}// namespace mlg
