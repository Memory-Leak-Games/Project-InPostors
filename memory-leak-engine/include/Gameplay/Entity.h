#pragma once

#include "SceneGraph/Transform.h"
#include "ComponentManager.h"
#include "Component.h"

namespace mlg {
    class Component;

    class Entity : public std::enable_shared_from_this<Entity> {
    private:
        std::string name;
        std::string tag;
        Transform transform;

        bool isStatic = false;
        bool isQueuedForDeletion = false;

        std::vector<std::weak_ptr<class Component>> components;

    public:
        Entity() = delete;
        explicit Entity(const std::string& name, bool isStatic, Transform* parent);

        template<typename T, typename ... Args>
        std::weak_ptr<T> AddComponent(Args&& ... args) {
            auto newComponent = ComponentManager::SpawnComponent<T>(shared_from_this(), std::forward<Args>(args)...);
            components.push_back(newComponent);
            return newComponent;
        }

        template<typename T>
        std::weak_ptr<T> GetComponentByClass() {
            auto foundIterator = std::find_if(components.begin(), components.end(),
                                              [](const std::weak_ptr<Component>& entry) {
                                                  return std::dynamic_pointer_cast<Component>(entry.lock()) != nullptr;
                                              });

            if (foundIterator != components.end())
                return *foundIterator;
            else
                return std::weak_ptr<T>();
        }

        template<typename T>
        std::weak_ptr<T> GetComponentByName(const std::string& name) {
            auto foundIterator = std::find_if(components.begin(), components.end(),
                                              [name](const std::weak_ptr<Component>& entry) {
                                                  return entry.lock()->GetName() == name;
                                              });

            if (foundIterator != components.end())
                return *foundIterator;
            else
                return std::weak_ptr<T>();
        }

        void RemoveComponent(Component* component);

        virtual void PhysicsUpdate();
        virtual void Update();
        virtual void LateUpdate();

        void QueueForDeletion();

        [[nodiscard]] bool IsQueuedForDeletion() const;
        [[nodiscard]] bool IsStatic() const;

        Transform& GetTransform();
        const std::string& GetName() const;
        const std::string& GetTag() const;

        void SetName(const std::string& name);
        void SetTag(const std::string& tag);

        virtual ~Entity() = 0;
    };

} // mlg
