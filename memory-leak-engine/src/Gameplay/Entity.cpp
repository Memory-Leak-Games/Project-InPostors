#include "Gameplay/Entity.h"

#include <utility>


namespace mlg {
    Entity::Entity(uint64_t id, std::string  name, bool isStatic, Transform* parent)
            : name(std::move(name)), isStatic(isStatic), transform(std::make_shared<Transform>()),
            id(id) {
        parent->AddChild(transform);
    }

    void Entity::Start() {}
    void Entity::PhysicsUpdate() {}
    void Entity::Update() {}
    void Entity::LateUpdate() {}
    void Entity::Stop() {}

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
        return *transform;
    }

    void Entity::QueueForDeletion() {
        isQueuedForDeletion = true;
        for (auto& component : components) {
            if (component.expired())
                continue;

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

    std::shared_ptr<Entity> Entity::Create(uint64_t id, const std::string& name, bool isStatic, Transform* parent) {
        return std::shared_ptr<Entity>(new Entity(id, name, isStatic, parent));
    }

    Entity::~Entity() = default;

    uint64_t Entity::GetId() const {
        return id;
    }

    bool Entity::operator==(const Entity &rhs) const {
        return id == rhs.id;
    }

    bool Entity::operator!=(const Entity &rhs) const {
        return !(rhs == *this);
    }

} // mlg