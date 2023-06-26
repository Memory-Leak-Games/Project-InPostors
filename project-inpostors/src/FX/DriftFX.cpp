#include "include/FX/DriftFX.h"
#include "Core/Time.h"

#include "Macros.h"
#include "Rendering/Particles/ParticleSystem.h"
#include "SceneGraph/Transform.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include <glm/fwd.hpp>

using Random = effolkronium::random_static;

DriftFX::DriftFX() : mlg::ParticleSystem(512) {
    SetMaterial(mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/particles/smoke_fx.json"));
}

void DriftFX::UpdateSystem(const class mlg::Transform& transform) {
    timeAccumulator += mlg::Time::GetDeltaSeconds();
}

void DriftFX::Emit(const class mlg::Transform& transform) {
    const float timeToEmit = 1.f / 60.f;

    if (timeAccumulator < timeToEmit) {
        return;
    }

    timeAccumulator = 0.f;

    mlg::ParticleProps particleProps{};
    particleProps.lifeTime = Random::get(0.3f, 0.6f);
    particleProps.position = transform.GetWorldPosition() - transform.GetForwardVector();

    particleProps.position += glm::vec3{Random::get(-1.f, 1.f), 0.f,
                                        Random::get(-1.f, 1.f)} *
                              0.3f;

    particleProps.beginSize = glm::vec2(0.5) * Random::get(0.8f, 1.f);
    particleProps.endSize = glm::vec2(0.5);
    particleProps.beginColor = HEX_RGBA(0x11111FF) * Random::get(0.8f, 1.f);
    particleProps.endColor = glm::vec4(1.f, 1.f, 1.f, 0.f) * 0.5f;

    particleProps.beginVelocity = glm::vec3(0.f);
    particleProps.endVelocity = glm::vec3(0.f);

    mlg::ParticleSystem::Emit(particleProps);
}

std::shared_ptr<mlg::ParticleSystem> DriftFX::Clone() {
    return std::make_shared<DriftFX>();
}
