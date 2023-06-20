#pragma once

#include "SceneGraph/Transform.h"

namespace mlg {

    class ISceneGraphClient {
    public:
        virtual Transform& GetTransform() = 0;
        virtual ~ISceneGraphClient() = default;
    };

}// namespace mlg