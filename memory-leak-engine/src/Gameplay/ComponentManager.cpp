#include "Gameplay/ComponentManager.h"

#include "Macros.h"

#include "Gameplay/Component.h"

namespace mlg {
    ComponentManager* ComponentManager::instance;

    void ComponentManager::Initialize() {
        if (instance != nullptr)
            return;

        instance = new ComponentManager();

        SPDLOG_INFO("Initializing Components Manager");
    }

    void ComponentManager::Stop() {
        SPDLOG_INFO("Stopping Components Manager");

        delete instance;
        instance = nullptr;
    }

    ComponentManager* ComponentManager::GetInstance() {
        return instance;
    }

    void ComponentManager::PhysicsUpdate() {
        for (const auto& component : instance->components) {
            component->PhysicsUpdate();
        }
    }

    void ComponentManager::Update() {
        for (const auto& component : instance->components) {
            component->Update();
        }
    }

    void ComponentManager::LateUpdate() {
        for (const auto& component : instance->components) {
            component->LateUpdate();
        }
    }

    void ComponentManager::ProcessComponents() {
        instance->components.erase(std::remove_if(instance->components.begin(), instance->components.end(),
                                                [](const std::weak_ptr<Component>& entry) {
                                                    return entry.lock()->IsQueuedForDeletion();
                                                }), instance->components.end());

    }
} // mlg