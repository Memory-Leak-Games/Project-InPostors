#pragma once

#include "soloud_wav.h"

namespace mlg {

    class Audio {
        std::string audioPath;
        SoLoud::Wav sound;
        uint32_t handle;

    public:
        Audio(const std::string& path);
        ~Audio();

        // Normal play
        void Play(float volume = 1.f,
                  float pan = 0.f,
                  uint32_t paused = 0,
                  uint32_t bus = 0);

        // Play without panning
        void PlayBackgroundMusic(float volume = 1.f,
                                 uint32_t paused = 0,
                                 uint32_t bus = 0);

        // TODO: Plays without a delay
        // Play with delay (time in sec)
        void PlayClocked(float delay,
                         float volume = 1.f,
                         float pan = 0.f,
                         uint32_t bus = 0);

        void Pause() const;
        void UnPause() const;

        void Seek(float time) const;

        void Stop();
        static void StopAll();

        void SetVolume(float volume);

        void SetLooping();
        // TODO: Check if these two work
        void SetSingleInstance();
        void SetFilter(uint32_t filterID, SoLoud::Filter* filter);

        double GetLength();
    };

}// namespace mlg
