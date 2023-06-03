#pragma once

#include <queue>
#include <Gameplay/Entity.h>

namespace mlg {
    class Entity;

    class EntityManager {
    private:
        static EntityManager* instance;

        std::vector<std::shared_ptr<Entity>> entities;
        uint64_t entitiesIndex = 1;

        EntityManager() = default;
    public:
        static void Initialize();
        static void Stop();
        static EntityManager* GetInstance();

        template<typename T, typename ... Args>
        static std::weak_ptr<T> SpawnEntity(Args&& ... args) {
            std::shared_ptr<Entity> newEntity = T::Create(instance->entitiesIndex, std::forward<Args>(args) ...);
            instance->entities.push_back(newEntity);
            instance->entitiesIndex++;
            return std::static_pointer_cast<T>(newEntity);
        }

        template<typename T>
        static std::vector<std::weak_ptr<Entity>> FindAllByPredicate(T predicate) {
            std::vector<std::weak_ptr<Entity>> foundElements;
            for (const auto& entity : instance->entities) {
                if (predicate(entity) == false)
                    continue;

                foundElements.push_back(entity);
            }
            return foundElements;
        }

        static std::weak_ptr<Entity> FindByName(const std::string& name);
        static std::vector<std::weak_ptr<Entity>> FindAllByTag(const std::string& tag);

        template<typename T>
        static std::vector<std::weak_ptr<T>> FindAllByType() {
            std::vector<std::weak_ptr<T>> foundElements;
            for (const auto& entity : instance->entities) {
                auto anotherEntity = std::dynamic_pointer_cast<T>(entity);
                if (anotherEntity != nullptr) {
                    foundElements.push_back(anotherEntity);
                }
            }
            return foundElements;
        }

        static void Start();
        static void AIUpdate();
        static void PhysicsUpdate();
        static void Update();
        static void LateUpdate();
        static void StopEntities();

        // For now is used to delete entities when every update is done
        static void ProcessEntities();
    };

} // mlg
