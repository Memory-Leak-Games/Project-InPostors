#include "Animation/Spawners/FerrisWheelAnimSpawner.h"

#include "Animation/SpecialAnimationComponent.h"

#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/Entity.h"

FerrisWheelAnimSpawner::FerrisWheelAnimSpawner() : AnimationSpawner() {}

void FerrisWheelAnimSpawner::Spawn(const std::shared_ptr<mlg::Entity>& entity, const std::shared_ptr<mlg::MaterialAsset>& material) {
    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/buildings/special/ferris_wheel_anim.obj");

    auto animMesh = entity->AddComponent<mlg::StaticMeshComponent>("AnimMesh", model, material);

    glm::vec3 position = entity->GetTransform().GetPosition();
    animMesh.lock()->GetTransform().SetPosition({ position.x, position.y + 9.45f, position.z });

    auto animComponent = entity->AddComponent<SpecialAnimationComponent>("AnimComponent", animMesh, rotate);
}
