#pragma once

#include "soloud_wavstream.h"

namespace mlg {

    class MusicAudio {
        std::string audioPath;
        SoLoud::WavStream sound;
        uint32_t handle;

    public:
        MusicAudio(const std::string& path);
        ~MusicAudio();

        void Play(SoLoud::Soloud* soloud);
        void PlayBackgroundMusic(SoLoud::Soloud* soloud);

        void Stop();
    };

}// namespace mlg
