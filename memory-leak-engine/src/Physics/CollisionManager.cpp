#include "Physics/CollisionManager.h"

#include "Macros.h"

namespace mlg {
    CollisionManager* CollisionManager::instance;

    void CollisionManager::Initialize() {
        if (instance != nullptr)
            return;

        instance = new CollisionManager();

        SPDLOG_INFO("Initializing CollisionManager");
    }

    void CollisionManager::Stop() {
        if (instance == nullptr)
            return;

        delete instance;
        instance = nullptr;

        SPDLOG_INFO("Stopping CollisionManager");
    }

    CollisionManager *CollisionManager::GetInstance() {
        return instance;
    }

    void CollisionManager::SolveCollisions() {
        // Doin' math
    }
} // mlg