#pragma once

namespace mlg {
    class MapObject {
    public:
        // ==== CONSTRUCTORS ====
        MapObject(const std::string& modelPath, const std::string& materialPath, float rot = 0.0f);
        MapObject(const std::string& modelPath, const std::string& materialPath,
                  std::string colType, float colSize = 1.0f, float colOffset = 0.0f,
                  float rot = 0.0f);

        // ==== METHODS ===
        void AddCollision(const std::string &colType, float colSize = 1.0f, float colOffset = 0.0f);

        // ==== GETTERS ====
        glm::vec3 GetRotation() { return worldRot; }
        std::weak_ptr<class ModelAsset> GetModel() { return model; }
        std::weak_ptr<class MaterialAsset> GetMaterial() { return material; }
        [[nodiscard]] bool HasCollision() const { return hasCollision; }
        [[nodiscard]] std::string GetColliderType() const { return colliderType; }
        [[nodiscard]] float GetColliderSize() const { return colliderSize; }
        [[nodiscard]] float GetColliderOffset() const { return colliderOffset; }

        // ==== SETTERS ====
        void SetRotation(const float rot) { worldRot = glm::vec3({0.0f, glm::radians(rot), 0.0f});};
        void SetRotation(const glm::vec3 rot) { worldRot = rot; }

    private:
        std::shared_ptr<ModelAsset> model;
        std::shared_ptr<MaterialAsset> material;
        glm::vec3 worldRot;
        std::string colliderType;
        float colliderSize = 1.0f;
        float colliderOffset = 0.0f;
        bool hasCollision;
    };
} // mlg

