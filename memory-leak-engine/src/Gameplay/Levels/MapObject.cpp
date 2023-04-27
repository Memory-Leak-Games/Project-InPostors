#include <utility>

#include "Gameplay/Levels/MapObject.h"
#include "Core/AssetManager/AssetManager.h"

#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

mlg::MapObject::MapObject(const std::string &modelPath, const std::string &materialPath, float rot) :
        model(mlg::AssetManager::GetAsset<mlg::ModelAsset>(modelPath)),
        material(mlg::AssetManager::GetAsset<mlg::MaterialAsset>(materialPath)),
        worldRot(glm::vec3({0.0f, glm::radians(rot), 0.0f})),
        hasCollision(false) {}


mlg::MapObject::MapObject(const std::string &modelPath, const std::string &materialPath,
                          std::string colType, float colSize, float colOffset, float rot) :
        model(mlg::AssetManager::GetAsset<mlg::ModelAsset>(modelPath)),
        material(mlg::AssetManager::GetAsset<mlg::MaterialAsset>(materialPath)),
        worldRot(glm::vec3({0.0f, glm::radians(rot), 0.0f})),
        hasCollision(true),
        colliderType(std::move(colType)),
        colliderSize(colSize),
        colliderOffset(colOffset) {}


void mlg::MapObject::AddCollision(const std::string &colType, float colSize, float colOffset) {
    hasCollision = true;
    colliderType = colType;
    colliderSize = colSize;
    colliderOffset = colOffset;
}
