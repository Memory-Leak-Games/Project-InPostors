#include "Audio/AudioAPI.h"

#include <iostream>

#include "Macros.h"

namespace mlg {
    AudioAPI* AudioAPI::instance;
    SoLoud::Soloud* AudioAPI::soloud;

    void AudioAPI::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing Audio API");

        soloud = new SoLoud::Soloud;
        soloud->init();

        std::cout << "\n    SoLoud Version: " << SOLOUD_VERSION << "\n\n";

        instance = new AudioAPI();
    }

    void AudioAPI::Stop() {
        SPDLOG_INFO("Stopping Audio API");

        soloud->deinit();
        delete soloud;

        delete instance;
        instance = nullptr;
    }

    AudioAPI* AudioAPI::GetInstance() {
        return instance;
    }

    SoLoud::Soloud* AudioAPI::GetSoLoud() {
        return soloud;
    }
}// namespace mlg