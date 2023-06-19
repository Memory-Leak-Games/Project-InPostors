#include "Gameplay/Components/ParticleSystemComponent.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Renderer.h"
#include "include/Rendering/Particles/ParticleSystem.h"

namespace mlg {
    ParticleSystemComponent::~ParticleSystemComponent() = default;

    ParticleSystem* ParticleSystemComponent::GetParticleSystem() const {
        return particleSystem.get();
    }

    ParticleSystemComponent::ParticleSystemComponent(
            const std::weak_ptr<Entity>& owner,
            const std::string& name,
            const std::shared_ptr<ParticleSystem>& particleSystem,
            bool autoDestroy)
        : particleSystem(particleSystem),
          SceneComponent(owner, name),
           autoDestroy(autoDestroy) {
        Renderer::GetInstance()->AddLateRenderable(particleSystem);
    }

    void ParticleSystemComponent::LateUpdate() {
        particleSystem->Update(GetTransform());

        if (autoDestroy && particleSystem->IsFinished())
            QueueForDeletion();
    }

    void ParticleSystemComponent::Emit(ParticleProps particleProps) {
        particleSystem->Emit(particleProps);
    }

}// namespace mlg