#include "Audio/AudioAPI.h"

#include <iostream>

#include "Macros.h"

namespace mlg {
    AudioAPI* AudioAPI::instance;
    SoLoud::Soloud* AudioAPI::soloudSFX;
//    SoLoud::Soloud* AudioAPI::soloudMusic;

    void AudioAPI::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing Audio API");

        soloudSFX = new SoLoud::Soloud;
//        soloudMusic = new SoLoud::Soloud;

        soloudSFX->init();
//        soloudMusic->init();

        std::cout << "\n    SoLoud Version: " << SOLOUD_VERSION << "\n\n";

        instance = new AudioAPI();
    }

    void AudioAPI::Stop() {
        SPDLOG_INFO("Stopping Audio API");

        soloudSFX->deinit();
//        soloudMusic->deinit();

        delete soloudSFX;
//        delete soloudMusic;

        delete instance;
        instance = nullptr;
    }

    AudioAPI* AudioAPI::GetInstance() {
        return instance;
    }

    SoLoud::Soloud* AudioAPI::GetSoLoudSFX() {
        return soloudSFX;
    }

//    SoLoud::Soloud* AudioAPI::GetSoLoudMusic() {
//        return soloudMusic;
//    }
}// namespace mlg