#include "FX/DestroyFX.h"

#include "Rendering/Assets/MaterialAsset.h"

using Random = effolkronium::random_static;

DestroyFX::DestroyFX()
    : mlg::ParticleSystem(15), firstUpdate(true) {
    SetMaterial(mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/particles/smoke_fx.json"));
}

void DestroyFX::UpdateSystem(const mlg::Transform& transform) {
    if (!firstUpdate)
        return;

    mlg::ParticleProps particleProps{};
    particleProps.beginVelocity = glm::vec3({0.f, 2.f, 0.f});
    particleProps.endVelocity = glm::vec3({0.f, 5.f, 0.f});

    for (int i = 0; i < 15; i++) {
        particleProps.lifeTime = Random::get(0.1f, 0.25f);
        particleProps.position = transform.GetWorldPosition();
        particleProps.position += glm::vec3{
            Random::get(-1.f, 1.f),
            1.f,
            Random::get(-0.f, 1.f)};

        particleProps.beginSize = glm::vec2(0.35) *
            Random::get(0.8f, 1.f);

        particleProps.endSize = glm::vec2(0.5);
        particleProps.beginColor = glm::vec4(1.f) * Random::get(0.5f, 1.f);
        particleProps.endColor = glm::vec4(1.f, 1.f, 1.f, 1.f) * 0.2f;
        particleProps.endColor.a = 1.f;

        Emit(particleProps);
    }

    firstUpdate = false;
}

std::shared_ptr<mlg::ParticleSystem> DestroyFX::Clone() {
    return std::make_shared<DestroyFX>();
}