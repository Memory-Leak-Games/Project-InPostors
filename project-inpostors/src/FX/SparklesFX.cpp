#include "FX/SparklesFX.h"

#include "Core/Math.h"
#include "Rendering/Assets/MaterialAsset.h"

using Random = effolkronium::random_static;

SparklesFX::SparklesFX()
    : mlg::ParticleSystem(5) {
    SetMaterial(mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/particles/smoke_fx.json"));
}

void SparklesFX::UpdateSystem(const mlg::Transform& transform) {
    if (finished)
        return;

    mlg::ParticleProps particleProps{};
    auto colors = std::array{
            HEX_RGBA(0xFFF304FF),
            HEX_RGBA(0xE1B530FF),
            };

    for (int i = 0; i < 3; i++) {
        particleProps.lifeTime = Random::get(1.f, 1.2f);
        particleProps.position = transform.GetWorldPosition();

        glm::vec3 direction = mlg::Math::GetRandomDirection();
        particleProps.beginVelocity = direction * Random::get(10.f, 15.f);
        particleProps.beginVelocity.y = 10.f;
        particleProps.endVelocity = particleProps.beginVelocity;
        particleProps.endVelocity.y = -15.f;

        particleProps.beginSize = glm::vec2(1.f) *
                                  Random::get(0.1f, 0.2f);
        particleProps.endSize = glm::vec2(0.1);

        particleProps.beginColor = *Random::get(colors);
        particleProps.endColor = particleProps.beginColor;

        Emit(particleProps);
    }

    finished = true;
}

std::shared_ptr<mlg::ParticleSystem> SparklesFX::Clone() {
    return std::make_shared<SparklesFX>();
}