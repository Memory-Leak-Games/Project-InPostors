#include "Audio/Audio.h"

namespace mlg {
    Audio::Audio(const std::string& path) : audioPath(path){
        sound.load(path.c_str());
    }

    Audio::~Audio() {
    }

    void Audio::Play(SoLoud::Soloud* soloud, float volume, float pan, uint32_t paused, uint32_t bus) {
        handle = soloud->play(sound, volume, pan, paused, bus);
    }

    void Audio::PlayBackgroundMusic(SoLoud::Soloud* soloud, float volume, uint32_t paused, uint32_t bus) {
        handle = soloud->playBackground(sound, volume, paused, bus);
    }

    void Audio::PlayClocked(SoLoud::Soloud* soloud, float delay, float volume, float pan, uint32_t bus) {
        handle = soloud->playClocked(delay, sound, volume, pan, bus);
    }

    void Audio::Pause(SoLoud::Soloud* soloud) const {
        soloud->setPause(handle, true);
    }

    void Audio::UnPause(SoLoud::Soloud* soloud) const {
        soloud->setPause(handle, false);
    }

    void Audio::Seek(SoLoud::Soloud* soloud, float time) const {
        soloud->seek(handle, time);
    }

    void Audio::Stop() {
        sound.stop();
    }

    void Audio::StopAll(SoLoud::Soloud* soloud) {
        soloud->stopAll();
    }

    void Audio::SetVolume(SoLoud::Soloud* soloud, float volume) {
        soloud->setVolume(handle, volume);
    }

    void Audio::SetLooping() {
        sound.setLooping(1);
    }

    void Audio::SetSingleInstance() {
        sound.setSingleInstance(1);
    }

    void Audio::SetFilter(uint32_t filterID, SoLoud::Filter* filter) {
        sound.setFilter(filterID, filter);
    }

    double Audio::GetLength() {
        return sound.getLength();
    }
}// namespace mlg