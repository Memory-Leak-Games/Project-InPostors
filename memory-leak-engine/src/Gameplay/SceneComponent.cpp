#include "Gameplay/SceneComponent.h"

#include "Gameplay/Entity.h"

namespace mlg {
    Transform& SceneComponent::GetTransform() {
        return *transform;
    }

    SceneComponent::SceneComponent(const std::weak_ptr<Entity>& owner, const std::string& name)
    : Component(owner, name), transform(std::make_shared<Transform>()) {
        auto ownerPtr = owner.lock();
        ownerPtr->GetTransform().AddChild(transform);
    }

    SceneComponent::~SceneComponent() = default;
} // mlg