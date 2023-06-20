#pragma once

#include "Component.h"
#include "SceneGraph/SceneGraphClient.h"
#include "SceneGraph/Transform.h"

namespace mlg {

    class SceneComponent : public Component, public ISceneGraphClient {
    private:
        std::shared_ptr<Transform> transform;

    public:
        SceneComponent(const std::weak_ptr<Entity>& owner, const std::string& name);
        ~SceneComponent() override = 0;

        Transform& GetTransform() override;
    };

}// namespace mlg
