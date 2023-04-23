#pragma once

#include "Gameplay/Component.h"
#include "soloud.h"

namespace mlg {

    class AudioComponent : public Component {
    private:
        std::shared_ptr<class AudioAsset> audio;

    public:
        AudioComponent(const std::weak_ptr<Entity>& owner, const std::string& name,
                       const std::shared_ptr<AudioAsset>& audio);

        void Play(SoLoud::Soloud* soloud);
    };

}// namespace mlg
