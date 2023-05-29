#pragma once

#include "Core/AssetManager/Asset.h"
#include "soloud.h"

namespace mlg {

    class AudioAsset : public Asset {
    private:
        std::unique_ptr<class Audio> audio;
    public:
        explicit AudioAsset(const std::string& path);
        ~AudioAsset() override;

        // Normal play
        void Play(float volume = 1.f,
                  float pan = 0.f,
                  uint32_t paused = 0,
                  uint32_t bus = 0);

        // Play without panning
        void PlayBackgroundMusic(float volume = 1.f,
                                 uint32_t paused = 0,
                                 uint32_t bus = 0);

        // Play with delay (time in sec)
        void PlayClocked(float delay,
                         float volume = 1.f,
                         float pan = 0.f,
                         uint32_t bus = 0);

        void Pause() const;
        void UnPause() const;

        void Seek(float time);

        void Stop();
        void StopAll();

        void SetVolume(float volume);
        void SetLooping();
        void SetSingleInstance();
        void SetFilter(uint32_t filterID, SoLoud::Filter* filter);

        double GetLength();

        void Load() override;
    };

}// namespace mlg
