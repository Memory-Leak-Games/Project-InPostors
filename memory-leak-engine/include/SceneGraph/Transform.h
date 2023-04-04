#pragma once

namespace mlg {

    class Transform {
    private:
        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rotation;

        glm::mat4 worldMatrix;
        glm::mat4 localMatrix;

        bool isDirty;

        std::vector<std::shared_ptr<Transform>> children;
        Transform* parent;
    public:
        Transform();
        Transform(const Transform& original);
        void operator= (const Transform& another);

        ~Transform();

        [[nodiscard]] const glm::vec3& GetPosition() const;
        [[nodiscard]] const glm::quat& GetRotation() const;
        [[nodiscard]] const glm::vec3 GetEulerRotation() const;
        [[nodiscard]] const glm::vec3& GetScale() const;

        [[nodiscard]] const glm::mat4& GetLocalMatrix();
        [[nodiscard]] const glm::mat4& GetWorldMatrix();

        [[nodiscard]] glm::vec3 GetWorldPosition();

        [[nodiscard]] glm::vec3 GetForwardVector();
        [[nodiscard]] glm::vec3 GetRightVector();
        [[nodiscard]] glm::vec3 GetUpVector();

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::quat& rotation);
        void SetEulerRotation(const glm::vec3 rotation);
        void SetScale(const glm::vec3& scale);

        const std::vector<std::shared_ptr<Transform>>& GetChildren();
        Transform* GetParent();

        void AddChild(const std::shared_ptr<Transform>& newChild);

        void Calculate();

    private:
        void Calculate(const glm::mat4&, bool isDirty) ;
        void SetDirtyRecursive();
        void ReCalculateParentRecursive() ;
    };

} // mlg
