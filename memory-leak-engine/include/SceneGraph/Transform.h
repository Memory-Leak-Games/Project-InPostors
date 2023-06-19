#pragma once

namespace mlg {

    class Transform {
    private:
        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rotation;

        mutable glm::mat4 worldMatrix;
        mutable glm::mat4 localMatrix;

        mutable bool isDirtyLocal;
        mutable bool isDirty;

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

        [[nodiscard]] const glm::mat4& GetLocalMatrix() const;
        [[nodiscard]] const glm::mat4& GetWorldMatrix() const;

        [[nodiscard]] glm::vec3 GetWorldPosition() const;

        [[nodiscard]] glm::vec3 GetForwardVector() const;
        [[nodiscard]] glm::vec3 GetRightVector() const;
        [[nodiscard]] glm::vec3 GetUpVector() const;

        [[nodiscard]] glm::vec3 InverseDirection(const glm::vec3& direction) const;

        [[nodiscard]] bool GetIsDirty() const;

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::quat& rotation);
        void SetEulerRotation(const glm::vec3& rotation);
        void SetScale(const glm::vec3& scale);

        void LookAt(const glm::vec3& lookAtPosition);

        const std::vector<std::weak_ptr<Transform>>& GetChildren();
        Transform* GetParent();
        void ReParent(Transform* newParent);

        void AddChild(const std::shared_ptr<Transform>& newChild);

        void Calculate() const;

    private:
        void Calculate(const glm::mat4&, bool isParentDirty) const;
        void SetDirtyRecursive();
        void ReCalculateParentRecursive() const;
    };

} // mlg
