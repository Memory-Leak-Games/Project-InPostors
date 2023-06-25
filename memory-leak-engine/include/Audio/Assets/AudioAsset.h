#pragma once

#include "Core/AssetManager/Asset.h"
#include "soloud_wav.h"
#include <cstdint>
#include <sys/types.h>

namespace SoLoud {
    class Filter;
}

namespace mlg {

    class AudioAsset : public Asset {
    private:
        SoLoud::Wav sound;
        uint32_t handle = 0;

    public:
        explicit AudioAsset(const std::string& path);
        ~AudioAsset() override;

        // Normal play
        uint32_t Play(float volume = 1.f,
                  float pan = 0.f,
                  uint32_t paused = 0,
                  uint32_t bus = 0);

        // Play without panning
        uint32_t PlayBackgroundMusic(float volume = 1.f,
                                 uint32_t paused = 0,
                                 uint32_t bus = 0);

        // Play with delay (time in sec)
        uint32_t PlayClocked(float delay,
                         float volume = 1.f,
                         float pan = 0.f,
                         uint32_t bus = 0);

        void Pause() const;
        void Pause(uint32_t handle) const;
        void UnPause() const;
        void UnPause(uint32_t handle) const;

        void Seek(float seconds);
        void Seek(uint32_t handle, float seconds);

        void Stop();
        void Stop(uint32_t handle);

        void SetVolume(float volume);
        void SetVolume(uint32_t handle, float volume);

        void SetLooping();
        void SetLooping(uint32_t handle);

        void SetSingleInstance();
        void SetFilter(uint32_t filterID, SoLoud::Filter* filter);

        double GetLength();

        void Load() override;
    };

}// namespace mlg
