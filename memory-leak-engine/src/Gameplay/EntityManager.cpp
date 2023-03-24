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

    void EntityManager::Start() {
        for (const auto& entity : instance->entities) {
            entity->Start();
        }
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

    std::weak_ptr<Entity> EntityManager::FindByName(const std::string& name) {
        auto foundIterator = std::find_if(instance->entities.begin(), instance->entities.end(),
                                          [name](const std::weak_ptr<Entity>& entry) {
                                              return entry.lock()->GetName() == name;
                                          });
        if (foundIterator == instance->entities.end())
            return {};

        return *foundIterator;
    }

    std::vector<std::weak_ptr<Entity>> EntityManager::FindAllByTag(const std::string& tag) {
        std::vector<std::weak_ptr<Entity>> foundElements;
        for (auto& entity : instance->entities) {
            if (entity->GetTag() != tag)
                continue;

            foundElements.push_back(entity);
        }

        return foundElements;
    }

} // mlg