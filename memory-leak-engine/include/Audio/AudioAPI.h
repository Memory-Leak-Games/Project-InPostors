#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

namespace mlg {

    class AudioAPI {
        static AudioAPI* instance;
        static ALCdevice* device;

        AudioAPI() = default;
    public:
        static void Initialize();
        static void Stop();

        static AudioAPI* GetInstance();
    private:
        static void OpenAlMessageCallback(unsigned source, unsigned type, unsigned id,
                                          unsigned severity, int length, const char* message,
                                          const void* userParam);
    };

}// namespace mlg
