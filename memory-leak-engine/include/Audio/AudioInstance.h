#pragma once

#include <memory>
namespace mlg {

    class AudioInstance {
    private:
        std::shared_ptr<class AudioAsset> audioAsset;
        uint32_t handle = 0;

    public:
        explicit AudioInstance(const std::shared_ptr<class AudioAsset>& audioAsset);
        explicit AudioInstance(const std::string& path);

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

        void Seek(float seconds);

        void Stop();
        void StopAll();

        void SetVolume(float volume);

        double GetLength();

        [[nodiscard]] std::shared_ptr<AudioAsset> GetAudioAsset() const;
    };
}