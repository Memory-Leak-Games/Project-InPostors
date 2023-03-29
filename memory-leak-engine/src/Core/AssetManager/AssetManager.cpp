
#include "Core/AssetManager/AssetManager.h"

namespace mlg {
    AssetManager* AssetManager::instance;

    void AssetManager::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing AssetManager");

        instance = new AssetManager;
    }

    void AssetManager::Stop() {
        SPDLOG_INFO("Stopping AssetManager");

        delete instance;
        instance = nullptr;
    }
}// namespace mlg