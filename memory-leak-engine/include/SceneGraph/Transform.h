#pragma once

namespace mlg {

    class Transform {
    private:
        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rotation;

        glm::mat4 worldMatrix;
        glm::mat4 localMatrix;

        bool isDirtyLocal;
        bool isDirty;

        std::vector<std::weak_ptr<Transform>> children;
        Transform* parent = nullptr;
    public:
        eventpp::CallbackList<void()> onTransformationChange;

        Transform();
        Transform(const Transform& another) = delete;

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

        [[nodiscard]] bool GetIsDirty() const;

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::quat& rotation);
        void SetEulerRotation(const glm::vec3& rotation);
        void SetScale(const glm::vec3& scale);

        void LookAt(const glm::vec3& lookAtPosition);

        const std::vector<std::weak_ptr<Transform>>& GetChildren();
        Transform* GetParent();

        void AddChild(const std::shared_ptr<Transform>& newChild);

        void Calculate();

    private:
        void Calculate(const glm::mat4&, bool isParentDirty);
        void SetDirtyRecursive();
        void ReCalculateParentRecursive() ;
    };

} // mlg
