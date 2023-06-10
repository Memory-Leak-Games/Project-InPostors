#pragma once

#include "Gameplay/Entity.h"
#include <cstddef>
namespace mlg {
    class ModelAsset;
    class MaterialAsset;
    class Entity;
}

struct EventPrefab {
    std::vector<std::shared_ptr<mlg::ModelAsset>> models;
    std::shared_ptr<mlg::MaterialAsset> material;
    glm::vec2 colliderSize;
    glm::vec2 colliderOffset;
};

struct GameplayEvent {
    size_t id;
    std::string prefabId;
    glm::vec2 position;
    bool vertical;

    const EventPrefab* eventPrefab;
};

class GameplayEventsManager {
private:
    std::unordered_map<std::string, EventPrefab> eventPrefabs;
    std::vector<GameplayEvent> events;

    size_t previousEventId = 0;
    int currentEventIndex = 0;

    std::weak_ptr<mlg::Entity> currentEventEntity;
public:
    explicit GameplayEventsManager(const std::string& levelPath);

    void TriggerEvent();

private:
    void LoadPrefabs(const nlohmann::json& prefabsJson);
    void LoadEvents(const nlohmann::json& eventsJson);
};