#include "Gameplay/Component.h"

#include <utility>

namespace mlg {
    Component::Component(std::weak_ptr<Entity> owner, std::string name)
    : owner(std::move(owner)), name(std::move(name)) {

    }

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
} // mlg