#include "Gameplay/Entity.h"


namespace mlg {
    Entity::Entity(const std::string& name, bool isStatic, Transform* parent)
            : name(name), isStatic(isStatic), transform() {
        parent->AddChild(transform.shared_from_this());
    }

    void Entity::PhysicsUpdate() {}

    void Entity::Update() {}

    void Entity::LateUpdate() {}

    bool Entity::IsQueuedForDeletion() const {
        return isQueuedForDeletion;
    }

    bool Entity::IsStatic() const {
        return isStatic;
    }

    const std::string& Entity::GetName() const {
        return name;
    }

    void Entity::SetName(const std::string& name) {
        Entity::name = name;
    }

    const std::string& Entity::GetTag() const {
        return tag;
    }

    void Entity::SetTag(const std::string& tag) {
        Entity::tag = tag;
    }

    Transform& Entity::GetTransform() {
        return transform;
    }

    void Entity::QueueForDeletion() {
        isQueuedForDeletion = true;
        for (auto& component : components) {
            component.lock()->QueueForDeletion();
        }
    }

    void Entity::RemoveComponent(Component* component) {
        auto foundIterator = std::find_if(components.begin(), components.end(),
                                          [component](const std::weak_ptr<Component>& entry) {
                                              return entry.lock().get() == component;
                                          });

        components.erase(foundIterator);
        component->QueueForDeletion();
    }
} // mlg