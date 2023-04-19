#pragma once

namespace SoLoud {
    class Soloud;
}

namespace mlg {

    class AudioAPI {
        static AudioAPI* instance;
        SoLoud::Soloud* soloudSFX;
//        static SoLoud::Soloud* soloudMusic;

        AudioAPI() = default;
    public:
        static void Initialize();
        static void Stop();

        static AudioAPI* GetInstance();
        static SoLoud::Soloud* GetSoLoudSFX();
//        static SoLoud::Soloud* GetSoLoudMusic();
    };

}// namespace mlg
