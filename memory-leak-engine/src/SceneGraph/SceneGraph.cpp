#include "SceneGraph/SceneGraph.h"

#include "Macros.h"
#include "include/SceneGraph/Transform.h"

namespace mlg {
    SceneGraph* SceneGraph::instance;

    void SceneGraph::Initialize() {
        if (instance != nullptr)
            return;

        instance = new SceneGraph();

        SPDLOG_INFO("Initializing SceneRoot");

        instance->root = std::make_unique<Transform>();
    }

    void SceneGraph::Stop() {
        delete instance;
        instance = nullptr;
    }

    void SceneGraph::CalculateGlobalTransforms() {
        instance->root->Calculate();
    }

    SceneGraph* SceneGraph::GetInstance() {
        return instance;
    }

    Transform* SceneGraph::GetRoot() {
        return instance->root.get();
    }
} // mlg