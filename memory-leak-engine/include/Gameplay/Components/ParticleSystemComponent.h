#pragma once

#include "Gameplay/SceneComponent.h"

#include "Rendering/Particles/Particles.h"

namespace mlg {

    class ParticleSystemComponent : public SceneComponent {
    private:
        bool autoDestroy = false;

    protected:
        std::shared_ptr<class ParticleSystem> particleSystem;

    public:
        ParticleSystemComponent() = delete;
        ParticleSystemComponent(
                const std::weak_ptr<Entity>& owner,
                const std::string& name,
                const std::shared_ptr<class ParticleSystem>& particleSystem,
                bool autoDestroy = false);

        ~ParticleSystemComponent();

        ParticleSystem* GetParticleSystem() const;

        void LateUpdate() override;
        void Emit(ParticleProps particleProps);
    };

}// namespace mlg
