#pragma once

#include "Core/Core.h"

namespace mlg {
    class Component;

    class ComponentManager {
    private:
        static ComponentManager* instance;

        std::vector<std::shared_ptr<Component>> components;

        ComponentManager() = default;
    public:
        static void Initialize();
        static void Stop();
        static ComponentManager* Get();

        template<typename T, typename ... Args>
        static std::weak_ptr<Component> SpawnComponent(std::shared_ptr<class Entity> owner, Args&& ... args) {
            auto newComponent = std::make_shared<T>(owner, std::forward<Args>(args) ...);
            instance->components.push_back(newComponent);

            if (mlg::Core::GetInstance()->IsClosed() == false)
                newComponent->Start();

            return newComponent;
        }

        static std::weak_ptr<Component> GetByRawPointer(Component* component);

        static void Start();

        static void AIUpdate();
        static void PhysicsUpdate();
        static void Update();
        static void LateUpdate();
        static void StopComponents();

        // For now is used to delete components when every update is done
        static void ProcessComponents();
    };

} // mlg
