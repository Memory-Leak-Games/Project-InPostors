#pragma once

namespace mlg {
    class ModelAsset;
}

struct EventPrefab {
    std::vector<std::shared_ptr<mlg::ModelAsset>> models;
    glm::vec2 colliderSize;
    glm::vec2 colliderOffset;
};

struct GameplayEvent {
    std::string prefabId;
    glm::vec2 position;
    bool vertical;
};

class GameplayEventsManager {
private:
    std::unordered_map<std::string, EventPrefab> eventPrefabs;
    std::vector<GameplayEvent> events;

    int eventCounter = 0;

public:
    explicit GameplayEventsManager(const std::string& levelPath);

    void TriggerEvent();

private:
    void LoadPrefabs(const nlohmann::json& prefabsJson);
    void LoadEvents(const nlohmann::json& eventsJson);
};