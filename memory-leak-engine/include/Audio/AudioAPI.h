#pragma once

#include "soloud.h"

namespace mlg {

    class AudioAPI {
        static AudioAPI* instance;
        static SoLoud::Soloud* soloud;

        AudioAPI() = default;
    public:
        static void Initialize();
        static void Stop();

        static AudioAPI* GetInstance();
        static SoLoud::Soloud* GetSoLoud();
    };

}// namespace mlg
