#pragma once

#include "SceneGraph/Transform.h"
#include "ComponentManager.h"
#include "Component.h"

#include "Macros.h"

namespace mlg {
    class Component;

    class Entity : public std::enable_shared_from_this<Entity> {
    private:
        uint64_t id;
        std::string name;
        std::string tag;
        std::shared_ptr<Transform> transform;

        bool isStatic = false;
        bool isQueuedForDeletion = false;

        std::vector<std::weak_ptr<class Component>> components;

        Entity() = delete;

    protected:
        explicit Entity(uint64_t id, std::string name, bool isStatic, Transform *parent);

    public:
        static std::shared_ptr<Entity> Create(uint64_t id, const std::string &name, bool isStatic, Transform *parent);
        virtual ~Entity();

        template<typename T, typename ... Args>
        std::weak_ptr<T> AddComponent(Args &&... args) {
            auto newComponent = ComponentManager::SpawnComponent<T>(shared_from_this(), std::forward<Args>(args)...);
            components.push_back(newComponent);
            return std::dynamic_pointer_cast<T>(newComponent.lock());
        }

        template<typename T>
        std::weak_ptr<T> GetComponentByClass() {
            auto foundIterator = std::find_if(
                    components.begin(), components.end(), [](const std::weak_ptr<Component> &entry) {
                        return std::dynamic_pointer_cast<T>(entry.lock()) != nullptr;
                    });

            if (foundIterator != components.end())
                return std::static_pointer_cast<T>(foundIterator->lock());
            else
                return std::weak_ptr<T>();
        }

        template<typename T>
        std::weak_ptr<T> GetComponentByName(const std::string &name) {
            auto foundIterator = std::find_if(components.begin(), components.end(),
                                              [name](const std::weak_ptr<Component> &entry) {
                                                  return entry.lock()->GetName() == name;
                                              });

            if (foundIterator != components.end())
                return std::static_pointer_cast<T>(foundIterator->lock());
            else
                return std::weak_ptr<T>();
        }

        void RemoveComponent(Component *component);

        virtual void Start();
        virtual void PhysicsUpdate();
        virtual void Update();
        virtual void LateUpdate();
        virtual void Stop();

        void QueueForDeletion();
        [[nodiscard]] bool IsQueuedForDeletion() const;

        [[nodiscard]] bool IsStatic() const;
        Transform &GetTransform();
        const std::string &GetName() const;
        const std::string &GetTag() const;
        uint64_t GetId() const;

        void SetName(const std::string &name);
        void SetTag(const std::string &tag);

        bool operator==(const Entity &rhs) const;
        bool operator!=(const Entity &rhs) const;

        struct Hash {
            size_t operator () (const Entity& entity) const {
                return entity.id;
            }
        };

        struct WeakHash {
            size_t operator () (const std::weak_ptr<Entity>& entity) const {
                return entity.lock()->id;
            }
        };

    };

    inline bool operator == (const std::weak_ptr<Entity>& lhs, const std::weak_ptr<Entity>& rhs) {
        return lhs.lock()->GetId() == rhs.lock()->GetId();
    }

    } // mlg
