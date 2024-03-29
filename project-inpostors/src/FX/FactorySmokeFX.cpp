#include "FX/FactorySmokeFX.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "SceneGraph/Transform.h"
#include "Core/Time.h"

FactorySmokeFX::FactorySmokeFX(): mlg::ParticleSystem(100) {
    SetMaterial(mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/particles/smoke_fx.json"));
}

void FactorySmokeFX::SetTimeToSpawn(float timeToSpawn) {
    this->timeToSpawn = timeToSpawn;
}

std::shared_ptr<mlg::ParticleSystem> FactorySmokeFX::Clone() {
    return std::make_shared<FactorySmokeFX>();
}

void FactorySmokeFX::UpdateSystem(const mlg::Transform& transform) {
    using Random = effolkronium::random_static;

    timeAccumulator += mlg::Time::GetDeltaSeconds();

    if (timeAccumulator > timeToSpawn)
    {
        mlg::ParticleProps particleProps {};
        particleProps.lifeTime = Random::get(1.0f, 1.5f);
        particleProps.position = transform.GetWorldPosition() - transform.GetForwardVector();

        particleProps.position += glm::vec3 {
                Random::get(-1.f, 1.f),
                1.f,
                Random::get(-1.f, 1.f)
        } * 0.2f;

        particleProps.beginSize = glm::vec2(1.4f) * Random::get(0.2f, 1.f);
        particleProps.endSize = glm::vec2(2.f) * Random::get(0.5f, 1.f);
        particleProps.beginColor = glm::vec4(glm::vec3(0.2f),1.f);
        particleProps.endColor = glm::vec4(1.f, 1.f, 1.f, 0.4f);

        particleProps.beginVelocity = glm::vec3({
                Random::get(-1.f, 1.f) + 2.f,
                20.f * Random::get(0.3f, 1.f),
                Random::get(-1.f, 1.f) + 2.f
        });

        particleProps.endVelocity = particleProps.beginVelocity * 0.8f;

        Emit(particleProps);

        timeAccumulator -= timeToSpawn;
    }
}
