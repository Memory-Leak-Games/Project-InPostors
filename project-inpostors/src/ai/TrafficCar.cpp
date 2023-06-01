#include "ai/TrafficCar.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/AssetManager/AssetManager.h"

#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "SceneGraph/Transform.h"

#include "Gameplay/Components/ParticleSystemComponent.h"

#include "FX/SmokeFX.h"

#include "Physics/Colliders/Collider.h"

#include "ai/AIComponent.h"

using json = nlohmann::json;

TrafficCar::TrafficCar(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent)
    : mlg::Entity(id, name, isStatic, parent){}

std::shared_ptr<TrafficCar> TrafficCar::Create(uint64_t id, const std::string& name, bool isStatic,
                                               mlg::Transform* parent, const TrafficCarData& trafficCarData,
                                               const std::string& configPath) {
    auto newTrafficCar = std::shared_ptr<TrafficCar>(new TrafficCar(id, name, isStatic, parent));

    newTrafficCar->rigidbodyComponent = newTrafficCar->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
    newTrafficCar->rigidbodyComponent.lock()->AddCollider<mlg::ColliderShape::Circle>(glm::vec2(0.f, -0.5f), 0.5f);
    newTrafficCar->rigidbodyComponent.lock()->AddCollider<mlg::ColliderShape::Circle>(glm::vec2(0.f, 0.5f), 0.5f);
    newTrafficCar->rigidbodyComponent.lock()->SetBounciness(0.5f);

    std::ifstream configFile{configPath};
    json configJson = json::parse(configFile);
    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>(configJson["model"].get<std::string>());
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(configJson["material"].get<std::string>());

    material = material->CreateDynamicInstance();
    material->SetVec4("color", trafficCarData.color);

    auto staticMeshComponent = newTrafficCar->AddComponent<mlg::StaticMeshComponent>("StaticMeshComponent", model, material);

    staticMeshComponent.lock()->GetTransform().SetPosition({0.f, 0.3f, 0.f});

    newTrafficCar->AddComponent<AIComponent>("AIComponent", configPath);

    auto smoke = std::make_shared<SmokeFX>();
    auto smokeComponent = newTrafficCar->AddComponent<mlg::ParticleSystemComponent>("SmokeFX", smoke);

    return newTrafficCar;
}

void TrafficCar::Start() {
}

void TrafficCar::Update() {
}

void TrafficCar::AIUpdate() {
}
