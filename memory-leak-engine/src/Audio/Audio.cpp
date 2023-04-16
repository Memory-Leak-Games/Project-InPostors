#include "Audio/Audio.h"

namespace mlg {
    Audio::Audio(const std::string& path) : audioPath(path){
        sound.load(path.c_str());
    }

    Audio::~Audio() {
    }

    void Audio::Play(SoLoud::Soloud* soloud) {
        handle = soloud->play(sound);
    }

    void Audio::Stop() {
        sound.stop();
    }
}// namespace mlg