#include "Animation/Spawners/CaffeeAnimSpawner.h"

#include "Animation/SpecialAnimationComponent.h"

#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/Entity.h"

CaffeeAnimSpawner::CaffeeAnimSpawner() : AnimationSpawner() {}

void CaffeeAnimSpawner::Spawn(const std::shared_ptr<mlg::Entity>& entity, const std::shared_ptr<mlg::MaterialAsset>& material) {
    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/buildings/special/caffee_anim.obj");

    auto animMesh = entity->AddComponent<mlg::StaticMeshComponent>("AnimMesh", model, material);

    auto animComponent = entity->AddComponent<SpecialAnimationComponent>("AnimComponent", animMesh, jump);
}
