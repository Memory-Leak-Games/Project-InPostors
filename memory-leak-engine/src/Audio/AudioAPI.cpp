#include "Audio/AudioAPI.h"

#include <iostream>
#include <soloud.h>

#include "Macros.h"

namespace mlg {
    AudioAPI* AudioAPI::instance;

    void AudioAPI::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing Audio API");
        instance = new AudioAPI();

        instance->soloud = new SoLoud::Soloud();

        instance->soloud->init();

        std::cout << "\n    SoLoud Version: " << SOLOUD_VERSION << "\n\n";

    }

    void AudioAPI::Stop() {
        SPDLOG_INFO("Stopping Audio API");

        instance->soloud->deinit();

        delete instance->soloud;

        delete instance;
        instance = nullptr;
    }

    AudioAPI* AudioAPI::GetInstance() {
        return instance;
    }

    SoLoud::Soloud* AudioAPI::GetSoLoud() {
        return instance->soloud;
    }
}// namespace mlg