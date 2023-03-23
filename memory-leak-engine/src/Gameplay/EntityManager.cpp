#include "Gameplay/EntityManager.h"
#include "Macros.h"

#include "Gameplay/Entity.h"

namespace mlg {
    EntityManager* EntityManager::instance;

    void EntityManager::Initialize() {
        if (instance != nullptr)
            return;

        instance = new EntityManager();

        SPDLOG_INFO("Initializing Entity Manager");
    }

    void EntityManager::Stop() {
        SPDLOG_INFO("Stopping Entity Manager");

        delete instance;
        instance = nullptr;
    }

    EntityManager* EntityManager::GetInstance() {
        return instance;
    }

    void EntityManager::PhysicsUpdate() {
        for (const auto& entity : instance->entities) {
            if (entity->IsStatic())
                continue;

            entity->PhysicsUpdate();
        }
    }

    void EntityManager::Update() {
        for (const auto& entity : instance->entities) {
            if (entity->IsStatic())
                continue;

            entity->Update();
        }
    }

    void EntityManager::LateUpdate() {
        for (const auto& entity : instance->entities) {
            if (entity->IsStatic())
                continue;

            entity->LateUpdate();
        }
    }

    void EntityManager::ProcessEntities() {
        instance->entities.erase(std::remove_if(instance->entities.begin(), instance->entities.end(),
                                         [](const std::weak_ptr<Entity>& entry) {
                                             return entry.lock()->IsQueuedForDeletion();
                                         }), instance->entities.end());
    }

} // mlg