#include "Managers/GameplayEventsManager.h"

#include "Core/AssetManager/AssetManager.h"
#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"

#include "Entities/GameplayEventEntity.h"
#include "SceneGraph/SceneGraph.h"

#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

#include <fstream>

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

    currentEventIndex++;

    // Shuffle events if all of them were triggered
    if (currentEventIndex >= events.size()) {
        currentEventIndex = 0;
        Random::shuffle(this->events);
    }

    // if event is the same as the previous, skip spawning it
    if (previousEventId == event.id)
        return;

    if (currentEventEntity.lock()) {
        currentEventEntity.lock()->QueueForDeletion();
    }

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
        gameplayEvent.prefabId = event["id"];
        gameplayEvent.position = {
                event["position"][0],
                event["position"][1]};
        gameplayEvent.vertical = event["vertical"];
        gameplayEvent.eventPrefab = &eventPrefabs[event["id"]];

        for (int i = 0; i < event["probability"]; i++)
            this->events.push_back(gameplayEvent);
    }

    Random::shuffle(this->events);
}
