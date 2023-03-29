#pragma once

#include "Component.h"
#include "SceneGraph/Transform.h"

namespace mlg {

    class SceneComponent : public Component {
    private:
        std::shared_ptr<Transform> transform;
    public:
        SceneComponent(const std::weak_ptr<Entity>& owner, const std::string& name);

        Transform& GetTransform();

        ~SceneComponent() override = 0;
    };

} // mlg
