#include "Buildings/Storage.h"

#include "Core/SceneManager/Scene.h"
#include "Core/SceneManager/SceneManager.h"
#include "Scenes/LevelScene.h"

#include "Gameplay/Components/RigidbodyComponent.h"

#include "Managers/ScoreManager.h"
#include "Managers/TaskManager.h"

#include "Rendering/Assets/MaterialAsset.h"

#include "UI/Assets/FontAsset.h"
#include "UI/Components/Image.h"
#include "Utils/EquipmentComponent.h"

#include <fstream>
#include <vector>

using json = nlohmann::json;

Storage::Storage(
        uint64_t id,
        const std::string& name,
        bool isStatic,
        mlg::Transform* parent)
    : InteractiveBuilding(id, name, isStatic, parent) {}

Storage::~Storage() = default;

std::shared_ptr<Storage> Storage::Create(
        uint64_t id, const std::string& name,
        bool isStatic, mlg::Transform* parent,
        const std::string& configPath) {
    auto storage = std::shared_ptr<Storage>(
            new Storage(id, name, isStatic, parent));

    std::ifstream configFile{configPath};
    json configJson = json::parse(configFile);

    std::string factoryType = configJson["type"];
    MLG_ASSERT(factoryType == "Storage");

    storage->AddMesh(configJson["static-mesh"]);
    auto mainRigidbody =
            storage->AddComponent<mlg::RigidbodyComponent>("MainRigidbody").lock();

    for (const auto& colliderJson : configJson["colliders"]) {
        storage->AddCollider(colliderJson, mainRigidbody.get());
    }

    for (const auto& emitterJson : configJson["emitters"]) {
        storage->AddEmitter(emitterJson);
    }

    storage->AddTrigger(
            configJson["input"],
            "input",
            mainRigidbody.get());

    storage->GenerateUI();
    mainRigidbody->SetKinematic(true);

    return storage;
}

bool Storage::TakeInputsFromInventory(class EquipmentComponent& equipment) {
    if (equipment.IsEmpty())
        return false;

    const std::vector<std::string> inputs = GetInputs();

    for (const auto& item : inputs) {
        if (!equipment.RequestProduct(item))
            continue;

        FinishTaskOrSellProduct(item);
        return true;
    }

    std::string product = equipment.RequestProduct();
    FinishTaskOrSellProduct(product);

    return true;
}

void Storage::FinishTaskOrSellProduct(const std::string& productId) {
    mlg::Scene* currentScene = mlg::SceneManager::GetCurrentScene();
    auto* levelScene = dynamic_cast<LevelScene*>(currentScene);

    // finish task
    if (levelScene->GetTaskManager()->FinishTask(productId))
        return;

    // or sell product
    levelScene->GetTaskManager()->SellProduct(productId);
}

std::vector<std::string> Storage::GetInputs() const {
    mlg::Scene* currentScene = mlg::SceneManager::GetCurrentScene();
    auto* levelScene = dynamic_cast<LevelScene*>(currentScene);

    std::vector<std::string> result;

    std::vector<TaskData> tasks = levelScene->GetTaskManager()->GetActiveTasks();
    result.reserve(tasks.size());

    for (const auto& task : tasks) {
        result.push_back(task.productId);
    }

    return result;
}

void Storage::GenerateUI() {
    auto font =
            mlg::AssetManager::GetAsset<mlg::FontAsset>(
                    "res/fonts/terminus-bold.ttf");
    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                    "res/materials/ui/factory/task_panel_material.json");

    auto temp = AddComponent<mlg::Image>(
                        "Storage", material)
                        .lock();

    temp->SetSize({64.f, 64.f});
    temp->SetBillboardTarget(shared_from_this());
    temp->SetPosition({0.f, 112.f});

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
            "res/materials/ui/icon/storage_material.json");

    temp = AddComponent<mlg::Image>("StorageIcon", material).lock();
    temp->SetSize({20.f, 20.f});
    temp->SetBillboardTarget(shared_from_this());
    temp->SetPosition({0.f, 112.f});
}
