#include "include/FX/SmokeFX.h"
#include "Core/Time.h"

#include "SceneGraph/Transform.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

using Random = effolkronium::random_static;

void SmokeFX::UpdateSystem(const mlg::Transform& transform) {
    const float timeToSpawn = 0.1f;

    timeAccumulator += mlg::Time::GetDeltaSeconds();

    if (timeAccumulator > timeToSpawn)
    {
        mlg::ParticleProps particleProps {};
        particleProps.lifeTime = Random::get(0.3f, 0.7f);
        particleProps.position = transform.GetWorldPosition() - transform.GetForwardVector();
        particleProps.position += glm::vec3 {Random::get(-1.f, 1.f), 1.f, Random::get(-1.f, 1.f)} * 0.2f;

        particleProps.beginSize = glm::vec2(0.35) * Random::get(0.8f, 1.f);
        particleProps.endSize = glm::vec2(0.5);
        particleProps.beginColor = glm::vec4(1.f);
        particleProps.endColor = glm::vec4(1.f, 1.f, 1.f, 0.f) * 0.5f;

        particleProps.beginVelocity = glm::vec3({0.f, 20.f, 0.f});
        particleProps.beginVelocity.y = 0.f;
        particleProps.endVelocity = glm::vec3(0.f);

        Emit(particleProps);

        timeAccumulator -= timeToSpawn;
    }
}

SmokeFX::SmokeFX() : mlg::ParticleSystem(100) {
    SetMaterial(mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/particles/smoke_fx.json"));
}

std::shared_ptr<mlg::ParticleSystem> SmokeFX::Clone() {
    return std::make_shared<SmokeFX>();
}
