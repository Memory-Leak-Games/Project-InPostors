#pragma once

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
        static ComponentManager* GetInstance();

        template<typename T, typename ... Args>
        static std::weak_ptr<T> SpawnComponent(Args&& ... args) {
            auto newComponent = std::make_shared<T>(std::forward<Args>(args) ...);
            instance->components.push_back(std::move(newComponent));
            return newComponent;
        }

        static void PhysicsUpdate();
        static void Update();
        static void LateUpdate();

        // For now is used to delete components when every update is done
        static void ProcessComponents();
    };

} // mlg
