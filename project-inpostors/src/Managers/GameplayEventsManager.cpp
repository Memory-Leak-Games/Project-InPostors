#include "Managers/GameplayEventsManager.h"

#include "Core/AssetManager/AssetManager.h"
#include "Core/Math.h"
#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"

#include "Entities/GameplayEventEntity.h"
#include "SceneGraph/SceneGraph.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"

#include "Core/Math.h"

#include <fstream>
#include <glm/fwd.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Random = effolkronium::random_static;

GameplayEventsManager::GameplayEventsManager(const std::string& levelPath) {
    std::ifstream levelFile{levelPath};
    json levelJson = json::parse(levelFile);

    std::ifstream prefabsFile{levelJson["events-config"]};
    json prefabsJson = json::parse(prefabsFile);

    LoadPrefabs(prefabsJson);
    LoadEvents(levelJson["events"]);
}

void GameplayEventsManager::TriggerEvent() {
    auto& event = events[currentEventIndex];

    if (IsPlayerOrNPCNearEvent(event))
        return;

    currentEventIndex++;

    // Shuffle events if all of them were triggered
    if (currentEventIndex >= events.size()) {
        currentEventIndex = 0;
        // Random::shuffle(this->events);
    }

    // if event is the same as the previous, skip spawning it
    if (previousEventId == event.id)
        return;

    if (currentEventEntity.lock()) {
        currentEventEntity.lock()->QueueForDeletion();
        previousEventId = 0;
    }

    if (event.skip)
        return;

    currentEventEntity = mlg::EntityManager::Get()->SpawnEntity<GameplayEventEntity>(
            fmt::format("Event_{}", currentEventIndex),
            false,
            mlg::SceneGraph::GetRoot(),
            event);

    previousEventId = event.id;
}

void GameplayEventsManager::LoadPrefabs(const nlohmann::json& prefabsJson) {
    for (auto& prefab : prefabsJson) {
        EventPrefab eventPrefab;
        eventPrefab.colliderSize = {
                prefab["collider"]["size"][0],
                prefab["collider"]["size"][1]};

        eventPrefab.colliderOffset = {
                prefab["collider"]["offset"][0],
                prefab["collider"]["offset"][1]};

        for (auto& mesh : prefab["meshes"]) {
            eventPrefab.models.push_back(
                    mlg::AssetManager::GetAsset<mlg::ModelAsset>(mesh));
        }
        eventPrefab.material =
                mlg::AssetManager::GetAsset<mlg::MaterialAsset>(prefab["material"]);

        eventPrefabs[prefab["id"]] = eventPrefab;
    }
}

void GameplayEventsManager::LoadEvents(const nlohmann::json& eventsJson) {
    size_t idCounter = 1;

    for (auto& event : eventsJson) {
        GameplayEvent gameplayEvent;
        gameplayEvent.id = idCounter++;

        gameplayEvent.skip = event["id"] == "None";

        if (event["id"] != "None") {
            gameplayEvent.prefabId = event["id"];
            gameplayEvent.eventPrefab = &eventPrefabs[event["id"]];

            gameplayEvent.position = {
                    event["position"][0],
                    event["position"][1]};
            gameplayEvent.vertical = event["vertical"];
        }

        for (int i = 0; i < event["probability"]; i++)
            this->events.push_back(gameplayEvent);
    }

    Random::shuffle(this->events);
}

bool GameplayEventsManager::IsPlayerOrNPCNearEvent(const GameplayEvent& event) const {
    const float nearDistance = 15.f;

    if (event.skip)
        return false;

    std::vector<std::weak_ptr<mlg::Entity>> entities = {
            mlg::EntityManager::Get()->FindByName("Player1"),
            mlg::EntityManager::Get()->FindByName("Player2")};

    auto npcs = mlg::EntityManager::FindAllByTag("NPC");
    entities.insert(entities.end(), npcs.begin(), npcs.end());

    for (auto& entity : entities) {
        if (entity.expired())
            continue;

        glm::vec3 entityPosition = entity.lock()->GetTransform().GetPosition();
        glm::vec3 eventPosition = mlg::Math::ProjectTo3D(event.position);

        float distance = glm::distance(entityPosition, eventPosition);

        if (distance < nearDistance)
            return true;
    }

    return false;
}
