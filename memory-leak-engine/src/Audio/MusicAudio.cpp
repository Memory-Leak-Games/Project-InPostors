#include "Audio/MusicAudio.h"

namespace mlg {
    MusicAudio::MusicAudio(const std::string& path) : audioPath(path){
        sound.load(path.c_str());
    }

    MusicAudio::~MusicAudio() {
    }

    void MusicAudio::Play(SoLoud::Soloud* soloud) {
        handle = soloud->play(sound);
    }

    void MusicAudio::PlayBackgroundMusic(SoLoud::Soloud* soloud) {
        handle = soloud->playBackground(sound);
    }

    void MusicAudio::Stop() {
        sound.stop();
    }
}// namespace mlg