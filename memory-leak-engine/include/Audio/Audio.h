#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

namespace mlg {

    class Audio {
        std::string audioPath;

    public:
        Audio(const std::string& path);
        ~Audio();

        void Play();
    };

}// namespace mlg
