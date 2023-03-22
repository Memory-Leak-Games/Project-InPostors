#pragma once


namespace mlg {
    class OldTransform {
    private:
        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rotation;

        bool isDirty;
    public:
        OldTransform();

        OldTransform(OldTransform *originalTransform);

        [[nodiscard]] glm::vec3 GetPosition() const;

        [[nodiscard]] glm::quat GetRotation() const;

        [[nodiscard]] glm::vec3 GetScale() const;

        void SetPosition(const glm::vec3 &newPosition);

        void SetRotation(const glm::quat &newRotation);

        void SetScale(const glm::vec3 &newScale);

        [[nodiscard]] glm::mat4 GetMatrix() const;

        friend class Node;
    };
}
