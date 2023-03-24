#include "Gameplay/Component.h"

#include <utility>

#include "Macros.h"

namespace mlg {
    Component::Component(std::weak_ptr<Entity> owner, std::string name)
    : owner(owner), name(std::move(name)) {
        MLG_ASSERT(owner.expired() == false, "Owner is not valid");
    }

    void Component::Start() {}
    void Component::PhysicsUpdate() {}
    void Component::Update() {}
    void Component::LateUpdate() {}

    bool Component::IsQueuedForDeletion() const {
        return isQueuedForDeletion;
    }

    const std::string& Component::GetName() const {
        return name;
    }

    void Component::SetName(const std::string& name) {
        Component::name = name;
    }

    void Component::QueueForDeletion() {
        isQueuedForDeletion = true;
    }

    Component::~Component() {}

    const std::weak_ptr<Entity>& Component::GetOwner() const {
        return owner;
    }

} // mlg