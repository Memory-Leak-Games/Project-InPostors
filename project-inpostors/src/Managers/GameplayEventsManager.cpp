#include "Managers/GameplayEventsManager.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/ModelAsset.h"

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
        eventPrefabs[prefab["id"]] = eventPrefab;
    }
}

void GameplayEventsManager::LoadEvents(const nlohmann::json& eventsJson) {
    for (auto& event : eventsJson) {
        GameplayEvent gameplayEvent;
        gameplayEvent.prefabId = event["id"];
        gameplayEvent.position = {
                event["position"][0],
                event["position"][1]};
        gameplayEvent.vertical = event["vertical"];

        for (int i = 0; i < event["probability"]; i++)
            this->events.push_back(gameplayEvent);
    }

    Random::shuffle(this->events);
}
