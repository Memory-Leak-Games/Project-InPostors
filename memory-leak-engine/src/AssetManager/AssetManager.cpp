
#include "Core/AssetManager/AssetManager.h"

namespace mlg {
    AssetManager* AssetManager::instance;

    void AssetManager::Initialize() {
        if (instance != nullptr)
            return;

        instance = new AssetManager;
    }

    void AssetManager::Stop() {
        delete instance;
        instance = nullptr;
    }
}// namespace mlg