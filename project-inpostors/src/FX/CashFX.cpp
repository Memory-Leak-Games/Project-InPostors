#include "FX/CashFX.h"

#include "Core/Math.h"
#include "Rendering/Assets/MaterialAsset.h"
#include <array>
#include <glm/fwd.hpp>

using Random = effolkronium::random_static;

CashFX::CashFX()
    : mlg::ParticleSystem(15) {
    SetMaterial(mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/particles/smoke_fx.json"));
}

void CashFX::UpdateSystem(const mlg::Transform& transform) {
    if (finished)
        return;

    mlg::ParticleProps particleProps{};
    auto colors = std::array{
            HEX_RGBA(0xA8AAB1FF),
            HEX_RGBA(0xFFF304FF),
            HEX_RGBA(0xE1B530FF),
            HEX_RGBA(0xA87D0DFF),
            };

    for (int i = 0; i < 25; i++) {
        particleProps.lifeTime = Random::get(0.5f, 0.8f);
        particleProps.position = transform.GetWorldPosition();

        glm::vec3 direction = mlg::Math::GetRandomDirection();
        particleProps.position += direction * Random::get(1.0f, 2.0f);
        particleProps.beginVelocity = direction * Random::get(10.f, 20.f);
        particleProps.endVelocity = particleProps.beginVelocity;
        particleProps.endVelocity.y = -15.f;

        particleProps.beginSize = glm::vec2(1.f) *
                                  Random::get(0.2f, 0.4f);
        particleProps.endSize = glm::vec2(0.1);

        particleProps.beginColor = *Random::get(colors);
        particleProps.endColor = particleProps.beginColor;

        Emit(particleProps);
    }

    finished = true;
}

std::shared_ptr<mlg::ParticleSystem> CashFX::Clone() {
    return std::make_shared<CashFX>();
}