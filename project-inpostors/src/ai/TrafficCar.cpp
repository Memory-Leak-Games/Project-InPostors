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

#include "FX/FXLibrary.h"
#include "ai/AIComponent.h"

using json = nlohmann::json;
using Random = effolkronium::random_static;

TrafficCar::TrafficCar(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent, const TrafficCarData& trafficCarData)
    : mlg::Entity(id, name, isStatic, parent), trafficCarData(trafficCarData) {}

std::shared_ptr<TrafficCar> TrafficCar::Create(uint64_t id, const std::string& name, bool isStatic,
                                               mlg::Transform* parent, const TrafficCarData& trafficCarData) {
    auto newTrafficCar = std::shared_ptr<TrafficCar>(new TrafficCar(id, name, isStatic, parent, trafficCarData));


    std::ifstream configFile{trafficCarData.carData};
    json configJson = json::parse(configFile);

    uint32_t modelNumber = Random::get<uint32_t>(1, 4);
    newTrafficCar->AddRigidbody(configJson, modelNumber);
    newTrafficCar->LoadModel(configJson, modelNumber);

    newTrafficCar->AddComponent<AIComponent>("AIComponent", trafficCarData.carData);
    newTrafficCar->SetTag("NPC");

    // Smoke looks weird on the jeep, we'll just say it's an electric car :)
    if (modelNumber == 2) {
        return newTrafficCar;
    }

    auto smoke = FXLibrary::Get("smoke");
    auto smokeComponent = newTrafficCar->AddComponent<mlg::ParticleSystemComponent>("SmokeFX", smoke);

    return newTrafficCar;
}

void TrafficCar::Start() {
}

void TrafficCar::Update() {
}

void TrafficCar::AIUpdate() {
}

void TrafficCar::AddRigidbody(const nlohmann::json& configJson, const uint32_t modelNumber) {
    this->rigidbodyComponent = this->AddComponent<mlg::RigidbodyComponent>("Rigidbody");

    float bounciness = configJson["parameters"].value("bounciness", 0.5f);
    this->rigidbodyComponent.lock()->SetBounciness(bounciness);


    for (const auto& collider : configJson["colliders" + std::to_string(modelNumber)]) {
        const glm::vec2 offset{
                collider["offset"][0].get<float>(),
                collider["offset"][1].get<float>(),
        };
        const float size = collider["size"].get<float>();

        this->rigidbodyComponent.lock()->AddCollider<mlg::ColliderShape::Circle>(offset, size);
    }
}

void TrafficCar::LoadModel(const nlohmann::json& configJson, const uint32_t modelNumber) {
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(configJson["material"]);

    material = material->CreateDynamicInstance();
    material->SetVec4("color", trafficCarData.color);

    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>(configJson["model" + std::to_string(modelNumber)]);
    auto staticMeshComponent = this->AddComponent<mlg::StaticMeshComponent>("StaticMeshComponent", model, material);

    staticMeshComponent.lock()->GetTransform().SetPosition({0.f, 0.3f, 0.f});
    staticMeshComponent.lock()->GetTransform().SetScale({0.7f, 0.7f, 0.7f});
}
