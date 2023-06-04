#pragma once

#include <Gameplay/Entity.h>

namespace mlg {
    class RigidbodyComponent;
}

struct TrafficCarData {
    int id;
    glm::vec4 color;
};

class TrafficCar : public mlg::Entity {
private:
    std::weak_ptr<mlg::RigidbodyComponent> rigidbodyComponent;

    TrafficCar(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);

public:
    static std::shared_ptr<TrafficCar> Create(uint64_t, const std::string& name, bool isStatic,
                                              mlg::Transform* parent, const TrafficCarData& trafficCarData,
                                              const std::string& configPath = "res/config/cars/traffic.json");

    void Start() override;
    void Update() override;
    void AIUpdate() override;

    ~TrafficCar() = default;

private:
    void AddRigidbody(const nlohmann::json& configJson, const uint32_t modelNumber);
    void LoadModel(const nlohmann::json& configJson);
};
