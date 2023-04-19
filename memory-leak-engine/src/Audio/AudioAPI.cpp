#include "Audio/AudioAPI.h"

#include <iostream>
#include <soloud.h>

#include "Macros.h"

namespace mlg {
    AudioAPI* AudioAPI::instance;
//    SoLoud::Soloud* AudioAPI::soloudMusic;

    void AudioAPI::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing Audio API");
        instance = new AudioAPI();

        instance->soloudSFX = new SoLoud::Soloud();
//        soloudMusic = new SoLoud::Soloud;

        instance->soloudSFX->init();
//        soloudMusic->init();

        std::cout << "\n    SoLoud Version: " << SOLOUD_VERSION << "\n\n";

    }

    void AudioAPI::Stop() {
        SPDLOG_INFO("Stopping Audio API");

        instance->soloudSFX->deinit();
//        soloudMusic->deinit();

        delete instance->soloudSFX;
//        delete soloudMusic;

        delete instance;
        instance = nullptr;
    }

    AudioAPI* AudioAPI::GetInstance() {
        return instance;
    }

    SoLoud::Soloud* AudioAPI::GetSoLoudSFX() {
        return instance->soloudSFX;
    }

//    SoLoud::Soloud* AudioAPI::GetSoLoudMusic() {
//        return soloudMusic;
//    }
}// namespace mlg