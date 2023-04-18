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

        void Play(SoLoud::Soloud* soloud);
        void PlayBackgroundMusic(SoLoud::Soloud* soloud);

        void Stop();
    };

}// namespace mlg
