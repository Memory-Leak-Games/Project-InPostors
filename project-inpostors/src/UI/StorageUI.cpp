#include "UI/StorageUI.h"

#include "Core/SceneManager/SceneManager.h"
#include "Core/Time.h"
#include "Gameplay/Entity.h"
#include "Managers/TaskManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Scenes/LevelScene.h"
#include "UI/Assets/FontAsset.h"
#include "UI/Components/Image.h"

StorageUI::StorageUI(const std::weak_ptr<mlg::Entity>& owner, const std::string& name)
    : SceneComponent(owner, name) {
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/storage/pin_material.json");
    pin = owner.lock()->AddComponent<mlg::Image>("StoragePin", material).lock();
    pin->SetSize({60.5f, 60.5f});
    pin->SetPosition({0.f, 50.f});
}

void StorageUI::Start() {
    auto sharedThis =
            mlg::ComponentManager::GetByRawPointer(this).lock();
    auto thisAsSceneComponent =
            std::static_pointer_cast<mlg::SceneComponent>(sharedThis);

    pin->SetBillboardTarget(thisAsSceneComponent);

    mlg::Scene* scene = mlg::SceneManager::GetCurrentScene();
    LevelScene* levelScene = dynamic_cast<LevelScene*>(scene);

    levelScene->GetTaskManager()->OnTaskFinished.append(
            [this](const TaskData& taskData) {
                animate = true;
                time = 0.f;
            });
}

void StorageUI::Update() {
    if (!animate)
        return;

    if (time > glm::pi<float>()) {
        animate = false;
        time = 0.f;
    }

    const float speed = 10.f;
    const float amplitude = 2.f;

    time += mlg::Time::GetDeltaSeconds() * speed;

    glm::vec3 position = GetTransform().GetPosition();
    position.y = glm::sin(time) * amplitude;
    GetTransform().SetPosition(position);
}
