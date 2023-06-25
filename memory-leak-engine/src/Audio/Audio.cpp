#include "Audio/Audio.h"

#include "Audio/AudioAPI.h"
#include "Macros.h"

namespace mlg {
    Audio::Audio(const std::string& path) : audioPath(path){
        sound.load(path.c_str());
    }

    Audio::~Audio() = default;

    void Audio::Play(float volume, float pan, uint32_t paused, uint32_t bus) {
        handle = mlg::AudioAPI::GetSoLoud()->play(sound, volume, pan, paused, bus);
    }

    void Audio::PlayBackgroundMusic(float volume, uint32_t paused, uint32_t bus) {
        handle = mlg::AudioAPI::GetSoLoud()->playBackground(sound, volume, paused, bus);
    }

    void Audio::PlayClocked(float delay, float volume, float pan, uint32_t bus) {
        handle = mlg::AudioAPI::GetSoLoud()->playClocked(delay, sound, volume, pan, bus);
    }

    void Audio::Pause() const {
        mlg::AudioAPI::GetSoLoud()->setPause(handle, true);
    }

    void Audio::UnPause() const {
        mlg::AudioAPI::GetSoLoud()->setPause(handle, false);
    }

    void Audio::Seek(float time) const {
        mlg::AudioAPI::GetSoLoud()->seek(handle, time);
    }

    void Audio::Stop() {
        sound.stop();
    }

    void Audio::StopAll() {
        mlg::AudioAPI::GetSoLoud()->stopAll();
    }

    void Audio::SetVolume(float volume) {
        mlg::AudioAPI::GetSoLoud()->setVolume(handle, volume);
    }

    void Audio::SetLooping() {
        sound.setLooping(true);
    }

    void Audio::SetSingleInstance() {
        sound.setSingleInstance(true);
    }

    void Audio::SetFilter(uint32_t filterID, SoLoud::Filter* filter) {
        sound.setFilter(filterID, filter);
    }

    double Audio::GetLength() {
        return sound.getLength();
    }
}// namespace mlg