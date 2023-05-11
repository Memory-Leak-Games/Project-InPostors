#include "Car/FX/SmokeFX.h"
#include "Core/Time.h"

#include "SceneGraph/Transform.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"

using Random = effolkronium::random_static;

void SmokeFX::UpdateSystem(const mlg::Transform& transform) {
    static float timeAccumulator;
    const float timeToSpawn = 0.05f;

    timeAccumulator += mlg::Time::GetDeltaSeconds();

    while (timeAccumulator > timeToSpawn)
    {
        mlg::ParticleProps particleProps {};
        particleProps.lifeTime = Random::get(0.3f, 0.7f);
        particleProps.position = transform.GetWorldPosition() - transform.GetForwardVector();
        particleProps.position += glm::vec3 {
                Random::get(-1.f, 1.f),
                1.f,
                Random::get(-1.f, 1.f)
        } * 0.2f;

        particleProps.beginSize = glm::vec2(0.35) * Random::get(0.8f, 1.f);
        particleProps.endSize = glm::vec2(0.5);
        particleProps.beginColor = glm::vec4(1.f);
        particleProps.endColor = glm::vec4(1.f, 1.f, 1.f, 0.f) * 0.5f;

        particleProps.beginVelocity = glm::vec3({
            Random::get(-4.f, 4.f),
            20.f,
            Random::get(-4.f, 4.f)
        });
        particleProps.beginVelocity.y = 0.f;
        particleProps.endVelocity = glm::vec3(0.f);

        Emit(particleProps);

        timeAccumulator -= timeToSpawn;
    }
}

SmokeFX::SmokeFX() : mlg::ParticleSystem(50) {
    SetMaterial(mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/Particles/smokeFX.json"));
}
