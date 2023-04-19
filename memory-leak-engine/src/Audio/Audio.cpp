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

    void Audio::PlayBackgroundMusic(SoLoud::Soloud* soloud) {
        handle = soloud->playBackground(sound);
    }

    void Audio::Stop() {
        sound.stop();
    }
}// namespace mlg