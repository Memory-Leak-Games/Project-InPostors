#pragma once

#include <queue>

namespace mlg {
    class Entity;

    class EntityManager {
    private:
        static EntityManager* instance;

        std::vector<std::shared_ptr<Entity>> entities;

        EntityManager() = default;
    public:
        static void Initialize();
        static void Stop();
        static EntityManager* GetInstance();

        template<typename T, typename ... Args>
        static std::weak_ptr<T> SpawnEntity(Args&& ... args) {
            auto newEntity = std::make_shared<T>(std::forward<Args>(args) ...);
            instance->entities.push_back(std::move(newEntity));
            return newEntity;
        }

        static void PhysicsUpdate();
        static void Update();
        static void LateUpdate();

        // For now is used to delete entities when every update is done
        static void ProcessEntities();
    };

} // mlg
