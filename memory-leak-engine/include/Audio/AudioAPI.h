#pragma once

namespace SoLoud {
    class Soloud;
}

namespace mlg {

    class AudioAPI {
        static AudioAPI* instance;
        SoLoud::Soloud* soloud;

        AudioAPI() = default;
    public:
        static void Initialize();
        static void Stop();

        void SetVolume(float volume);

        static AudioAPI* Get();
        static SoLoud::Soloud* GetSoLoud();
    };

}// namespace mlg
