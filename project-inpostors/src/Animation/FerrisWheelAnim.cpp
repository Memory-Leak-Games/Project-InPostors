#include "Animation/FerrisWheelAnim.h"

#include "Animation/SpecialAnimationComponent.h"

#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "Gameplay/Components/StaticMeshComponent.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"

#include "SceneGraph/SceneGraph.h"

FerrisWheelAnim::FerrisWheelAnim() {}

void FerrisWheelAnim::Spawn(AnimatedEntityData* animData) {
    auto newFerrisWheel = mlg::EntityManager::SpawnEntity<mlg::Entity>(
                                  "FerrisWheel", !animData->mapObject->isDynamic, mlg::SceneGraph::GetRoot())
                                  .lock();

    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>(animData->mapObject->modelPath);
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(animData->mapObject->materialPath);

    auto staticMeshComponent = newFerrisWheel->AddComponent<mlg::StaticMeshComponent>("BaseStaticMesh", model, material);

    model = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/buildings/special/ferris_wheel_anim.obj");
    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(animData->mapObject->materialPath);

    auto animMesh = newFerrisWheel->AddComponent<mlg::StaticMeshComponent>("AnimMesh", model, material);

    newFerrisWheel->GetTransform().SetPosition(animData->objectPosition);
    newFerrisWheel->GetTransform().SetEulerRotation({0.f, animData->mapObject->worldRot + animData->rotation, 0.f});
    newFerrisWheel->GetTransform().SetScale(glm::vec3{animData->mapObject->scale});

    auto rigidbody = newFerrisWheel->AddComponent<mlg::RigidbodyComponent>("Rigidbody").lock();

    std::string colType = animData->mapObject->colliderType;
    if (colType == "rectangle") {
        rigidbody->AddCollider<mlg::ColliderShape::Rectangle>(
                glm::vec2(animData->mapObject->colliderOffset),
                glm::vec2(animData->mapObject->colliderSize));
    } else if (colType == "circle") {
        rigidbody->AddCollider<mlg::ColliderShape::Circle>(
                glm::vec2(glm::vec2(animData->mapObject->colliderSize)),
                animData->mapObject->colliderOffset);
    }

    rigidbody->SetRotation(animData->mapObject->worldRot + animData->rotation);
    rigidbody->SetKinematic(!animData->mapObject->isDynamic);

    rigidbody->SetLinearDrag(animData->mapObject->linearDrag);
    rigidbody->SetAngularDrag(animData->mapObject->angularDrag);

    auto animComponent = newFerrisWheel->AddComponent<SpecialAnimationComponent>("AnimComponent", animMesh);
}
