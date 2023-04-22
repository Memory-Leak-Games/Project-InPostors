#include <tracy/Tracy.hpp>
#include "Gameplay/ComponentManager.h"

#include "Macros.h"

#include "Gameplay/Component.h"
#include "Physics/Physics.h"

namespace mlg {
    ComponentManager* ComponentManager::instance;

    void ComponentManager::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing Components Manager");

        instance = new ComponentManager();

        Physics::GetInstance()->OnFixedUpdate.append(ComponentManager::PhysicsUpdate);
    }

    void ComponentManager::Stop() {
        SPDLOG_INFO("Stopping Components Manager");

        StopComponents();

        delete instance;
        instance = nullptr;
    }

    ComponentManager* ComponentManager::GetInstance() {
        return instance;
    }

    void ComponentManager::PhysicsUpdate() {
        ZoneScopedNC("Component Physics Update", tracy::Color::ColorType::Green);

        for (const auto& component : instance->components) {
            component->PhysicsUpdate();
        }
    }

    void ComponentManager::Start() {
        for (const auto& component : instance->components) {
            component->Start();
        }
    }

    void ComponentManager::Update() {
        ZoneScopedN("Component Update");

        for (const auto& component : instance->components) {
            component->Update();
        }
    }

    void ComponentManager::LateUpdate() {
        ZoneScopedN("Component LateUpdate");

        for (const auto& component : instance->components) {
            component->LateUpdate();
        }
    }

    void ComponentManager::StopComponents() {
        for (const auto& component : instance->components) {
            component->Stop();
        }
    }

    void ComponentManager::ProcessComponents() {
        ZoneScopedN("Component Process");
        instance->components.erase(std::remove_if(instance->components.begin(), instance->components.end(),
                                                  [](const std::shared_ptr<Component>& entry) {
                                                      if (entry->IsQueuedForDeletion()) {
                                                          entry->Stop();
                                                          return true;
                                                      }

                                                      return false;
                                                  }), instance->components.end());

    }

    std::weak_ptr<Component> ComponentManager::GetByRawPointer(Component* component) {
        auto foundIterator = std::find_if(instance->components.begin(), instance->components.end(),
                                          [component](const std::weak_ptr<Component>& entry) {
                                              return entry.lock().get() == component;
                                          });

        if (foundIterator == instance->components.end())
            return {};

        return *foundIterator;
    }
} // mlg