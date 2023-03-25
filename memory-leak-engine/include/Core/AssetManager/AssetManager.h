#pragma once

#include "Macros.h"
#include "Core/AssetManager/Asset.h"

namespace mlg {

    class AssetManager {
    private:
        static AssetManager* instance;

        // path, asset
        std::unordered_map<std::string, std::weak_ptr<Asset>> cacheAssetMap;

    public:
        static void Initialize();
        static void Stop();

        template<typename T>
        static std::shared_ptr<T> GetAsset(const std::string& path) {
            MLG_ASSERT_MSG((std::is_base_of<Asset, T>::value), "Assets must be derived from Asset class");

            auto iterator = instance->cacheAssetMap.find(path);

            if (iterator == instance->cacheAssetMap.end() || iterator->second.expired()) {
                auto newAsset = std::make_shared<T>(path);
                newAsset->Load();
                instance->cacheAssetMap[path] = newAsset;

                return newAsset;
            }

            return std::static_pointer_cast<T>(iterator->second.lock());
        }
    };

}// namespace mlg
