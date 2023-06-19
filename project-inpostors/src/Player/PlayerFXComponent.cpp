#include "Player/PlayerFXComponent.h"

#include "FX/FXLibrary.h"
#include "Gameplay/Components/ParticleSystemComponent.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Entity.h"
#include "Macros.h"

PlayerFXComponent::PlayerFXComponent(
        const std::weak_ptr<mlg::Entity>& owner, const std::string& name)
    : Component(owner, name) {
    auto sharedOwner = owner.lock();

    rigidbodyComponent =
            sharedOwner->GetComponentByClass<mlg::RigidbodyComponent>().lock();

    auto smoke = FXLibrary::Get("smoke");
    smokeParticleSystem =
            sharedOwner->AddComponent<mlg::ParticleSystemComponent>(
                    "SmokeFX", smoke);
    smokeParticleSystem.lock()->GetTransform().SetPosition(
            glm::vec3(0.0f, 0.5f, 0.0f));
}

PlayerFXComponent::~PlayerFXComponent() = default;

void PlayerFXComponent::Start() {
}

void PlayerFXComponent::Update() {
}
