#include "Audio/AudioAPI.h"

#include <iostream>

#include "Macros.h"

namespace mlg {
    AudioAPI* AudioAPI::instance;

    void AudioAPI::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing Audio API");

        instance = new AudioAPI();

        device = alcOpenDevice(NULL);

        MLG_ASSERT_MSG(device, "Failed to initialize OpenAL Device");

        std::cout << "\nOpenAL Info:\n"
                  << "\tVendor: " << alGetString(AL_VENDOR) << "\n"
                  << "\tRenderer: " << alGetString(AL_RENDERER) << "\n"
                  << "\tVersion: " << alGetString(AL_VERSION) << "\n"
                  << "\tExtensions: " << alGetString(AL_EXTENSIONS) << "\n\n";
    }

    void AudioAPI::Stop() {
        SPDLOG_INFO("Stopping Audio API");

        delete instance;
        instance = nullptr;
    }

    AudioAPI* AudioAPI::GetInstance() {
        return instance;
    }

    void AudioAPI::OpenAlMessageCallback(unsigned int source, unsigned int type, unsigned int id,
                                         unsigned int severity, int length, const char* message,
                                         const void* userParam) {
        switch (type) {
            case AL_INVALID_NAME:
                SPDLOG_ERROR("OpenAL error: {}", message);
                return;
            case AL_INVALID_ENUM:
                SPDLOG_ERROR("OpenAL error: {}", message);
                return;
            case AL_INVALID_VALUE:
                SPDLOG_ERROR("OpenAL error: {}", message);
                return;
            case AL_INVALID_OPERATION:
                SPDLOG_ERROR("OpenAL error: {}", message);
                return;
            case AL_OUT_OF_MEMORY:
                SPDLOG_ERROR("OpenAL error: {}", message);
                return;
            default:
                SPDLOG_ERROR("OpenAL: {}", message);
                return;
        }
    }
}// namespace mlg