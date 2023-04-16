#pragma once

#include "soloud_wav.h"

namespace mlg {

    class Audio {
        std::string audioPath;
        SoLoud::Wav sound;
        int handle;

    public:
        Audio(const std::string& path);
        ~Audio();

        void Play(SoLoud::Soloud* soloud);

        void Stop();
    };

}// namespace mlg
