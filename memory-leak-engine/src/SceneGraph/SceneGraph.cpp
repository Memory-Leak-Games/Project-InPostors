#include "SceneGraph/SceneGraph.h"

#include "Macros.h"
#include "include/SceneGraph/Transform.h"

namespace mlg {
    Transform* SceneGraph::root;

    void SceneGraph::Initialize() {
        if (root != nullptr)
            return;

        SPDLOG_INFO("Initializing SceneRoot");

        root = new Transform;
    }

    void SceneGraph::Stop() {
        if (root == nullptr)
            return;

        SPDLOG_INFO("Deleting SceneRoot");

        delete root;
    }

    void SceneGraph::CalculateGlobalTransforms() {
        root->Calculate();
    }

    Transform* SceneGraph::GetRoot() {
        return root;
    }
} // mlg