#include "Animation/Spawners/FanAnimSpawner.h"

#include "Animation/SpecialAnimationComponent.h"

#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/Entity.h"

FanAnimSpawner::FanAnimSpawner() : AnimationSpawner() {}

void FanAnimSpawner::Spawn(const std::shared_ptr<mlg::Entity>& entity, const std::shared_ptr<mlg::MaterialAsset>& material) {
    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/buildings/special/fan_anim.obj");

    auto animMesh = entity->AddComponent<mlg::StaticMeshComponent>("AnimMesh", model, material);

    glm::vec3 position = entity->GetTransform().GetPosition();
    animMesh.lock()->GetTransform().SetPosition({ position.x, position.y + 17.2f, position.z });

    auto animComponent = entity->AddComponent<SpecialAnimationComponent>("AnimComponent", animMesh, rotate);
}
