#include "Entities/GameplayEventEntity.h"

#include "Core/AssetManager/AssetManager.h"
#include "Core/Math.h"

#include "Gameplay/Components/StaticMeshComponent.h"
#include "Managers/GameplayEventsManager.h"

using Random = effolkronium::random_static;

GameplayEventEntity::GameplayEventEntity(
        uint64_t id, const std::string& name,
        bool isStatic, mlg::Transform* parent)
    : Entity(id, name, isStatic, parent) {}

GameplayEventEntity::~GameplayEventEntity() = default;

std::shared_ptr<GameplayEventEntity> GameplayEventEntity::Create(
        uint64_t id, const std::string& name,
        bool isStatic, mlg::Transform* parent,
        const class GameplayEvent& event) {

    auto gameplayEvent = std::shared_ptr<GameplayEventEntity>(
            new GameplayEventEntity(id, name, isStatic, parent));

    gameplayEvent->AddMesh(event);
    gameplayEvent->AddCollider(event);

    gameplayEvent->GetTransform().SetPosition(
            mlg::Math::ProjectTo3D(event.position));

    return gameplayEvent;
}


void GameplayEventEntity::AddMesh(const GameplayEvent& event) {
    auto mesh = AddComponent<mlg::StaticMeshComponent>(
            "EventMesh",
            *Random::get(event.eventPrefab->models),
            event.eventPrefab->material);

    if (event.vertical) {
        mesh.lock()->GetTransform().SetEulerRotation(
                glm::vec3{0.f, glm::radians(90.f), 0.f});
    }
}

void GameplayEventEntity::AddCollider(const GameplayEvent& event) {
    SPDLOG_ERROR("UNIMPLEMENTED");
}
