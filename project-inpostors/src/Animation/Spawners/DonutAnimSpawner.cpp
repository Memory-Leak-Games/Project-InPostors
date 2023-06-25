#include "Animation/Spawners/DonutAnimSpawner.h"

#include "Animation/SpecialAnimationComponent.h"

#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/Entity.h"

DonutAnimSpawner::DonutAnimSpawner() : AnimationSpawner() {}

void DonutAnimSpawner::Spawn(const std::shared_ptr<mlg::Entity>& entity, const std::shared_ptr<mlg::MaterialAsset>& material) {
    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/buildings/special/donut_anim.obj");

    auto animMesh = entity->AddComponent<mlg::StaticMeshComponent>("AnimMesh", model, material);

    glm::vec3 position = entity->GetTransform().GetPosition();
    animMesh.lock()->GetTransform().SetPosition({ position.x, position.y + 2.08f, position.z + 1.7f});

//    auto animComponent = entity->AddComponent<SpecialAnimationComponent>("AnimComponent", animMesh, rotateYZ);
}
