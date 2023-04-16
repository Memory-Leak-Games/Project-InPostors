#include "Audio/AudioAPI.h"

#include <iostream>

#include "Macros.h"

namespace mlg {
    AudioAPI*   AudioAPI::instance;
//    ALCdevice*  AudioAPI::device;
//    ALCcontext* AudioAPI::context;

    void AudioAPI::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing Audio API");

        instance = new AudioAPI();

        std::cout << "\nOpenAL Info:\n"
                  << "\tVendor: " << alGetString(AL_VENDOR) << "\n"
                  << "\tRenderer: " << alGetString(AL_RENDERER) << "\n"
                  << "\tVersion: " << alGetString(AL_VERSION) << "\n"
                  << "\tExtensions: " << alGetString(AL_EXTENSIONS) << "\n\n";
    }

    void AudioAPI::Stop() {
        SPDLOG_INFO("Stopping Audio API");

//        context = alcGetCurrentContext();
//        device = alcGetContextsDevice(context);
//        alcMakeContextCurrent(nullptr);
//        alcDestroyContext(context);
//        alcCloseDevice(device);

        delete instance;
        instance = nullptr;
    }

    void AudioAPI::Setup() {
//        device = alcOpenDevice(nullptr);
//
//        MLG_ASSERT_MSG(!device, "Failed to initialize OpenAL Device");
//
//        context = alcCreateContext(device, nullptr);
//
//        MLG_ASSERT_MSG(context, "Failed to initialize OpenAL Context");
    }

    AudioAPI* AudioAPI::GetInstance() {
        return instance;
    }

    void AudioAPI::OpenALErrorCheck(const std::string& filename, const std::uint_fast32_t line) {
        ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            switch (error) {
                case AL_INVALID_NAME:
                    SPDLOG_ERROR("OpenAL error at {} - {}: {}", filename, line, "AL_INVALID_NAME");
                    return;
                case AL_INVALID_ENUM:
                    SPDLOG_ERROR("OpenAL error at {} - {}: {}", filename, line, "AL_INVALID_ENUM");
                    return;
                case AL_INVALID_VALUE:
                    SPDLOG_ERROR("OpenAL error at {} - {}: {}", filename, line, "AL_INVALID_VALUE");
                    return;
                case AL_INVALID_OPERATION:
                    SPDLOG_ERROR("OpenAL error at {} - {}: {}", filename, line, "AL_INVALID_OPERATION");
                    return;
                case AL_OUT_OF_MEMORY:
                    SPDLOG_ERROR("OpenAL error at {} - {}: {}", filename, line, "AL_OUT_OF_MEMORY");
                    return;
                default:
                    SPDLOG_ERROR("OpenAL at {} - {}: {}", filename, line, "Unknown AL error");
                    return;
            }
        }
    }

    void AudioAPI::OpenALCErrorCheck(const std::string& filename, const std::uint_fast32_t line) {
//        ALCenum error = alcGetError(device);
        ALCenum error = alGetError();
        if (error != ALC_NO_ERROR) {
            switch (error) {
                case ALC_INVALID_VALUE:
                    SPDLOG_ERROR("OpenAL error at {} - {}: {}", filename, line, "ALC_INVALID_VALUE");
                    return;
                case ALC_INVALID_DEVICE:
                    SPDLOG_ERROR("OpenAL error at {} - {}: {}", filename, line, "ALC_INVALID_DEVICE");
                    return;
                case ALC_INVALID_CONTEXT:
                    SPDLOG_ERROR("OpenAL error at {} - {}: {}", filename, line, "ALC_INVALID_CONTEXT");
                    return;
                case ALC_INVALID_ENUM:
                    SPDLOG_ERROR("OpenAL error at {} - {}: {}", filename, line, "ALC_INVALID_ENUM");
                    return;
                case ALC_OUT_OF_MEMORY:
                    SPDLOG_ERROR("OpenAL error at {} - {}: {}", filename, line, "ALC_OUT_OF_MEMORY");
                    return;
                default:
                    SPDLOG_ERROR("OpenAL at {} - {}: {}", filename, line, "Unknown ALC error");
                    return;
            }
        }
    }
}// namespace mlg