#include "Gameplay/Components/ParticleSystemComponent.h"

#include "include/Rendering/Particles/ParticleSystem.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Renderer.h"

namespace mlg {
    ParticleSystemComponent::~ParticleSystemComponent() = default;

    ParticleSystemComponent::ParticleSystemComponent(const std::weak_ptr<Entity>& owner, const std::string& name,
                                                     const std::shared_ptr<ParticleSystem>& particleSystem)
    : particleSystem(particleSystem), SceneComponent(owner, name) {
        Renderer::GetInstance()->AddLateRenderable(particleSystem);
    }

    void ParticleSystemComponent::LateUpdate() {
        particleSystem->Update(GetTransform());
    }

    void ParticleSystemComponent::Emit(ParticleProps particleProps) {
        particleSystem->Emit(particleProps);
    }

} // mlg